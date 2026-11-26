#!/bin/sh
# Check that Cyrus is in a good state, using the standalone (vendored Qt) build.
#   1. build  - ./build.sh --standalone succeeds
#   2. tests  - every ctest test passes
#   3. run    - build-standalone/cyrus stays up for 5s with no errors logged
# Exits non-zero if any step fails. Logs are kept in build-standalone/check/.
set -u

cd "$(dirname "$0")"

build_dir=build-standalone
log_dir=$build_dir/check
run_seconds=5
failed=0

result() {
    if [ "$1" -eq 0 ]; then echo "  PASS"; else echo "  FAIL"; failed=1; fi
    echo
}

mkdir -p "$log_dir"

# ---- 1. Build ----
echo "[1/3] Building standalone ($build_dir/)..."
./build.sh --standalone >"$log_dir/build.log" 2>&1
build_status=$?
if [ "$build_status" -eq 0 ]; then
    echo "  Build status: succeeded"
else
    echo "  Build status: failed (exit $build_status), last lines of $log_dir/build.log:"
    tail -n 20 "$log_dir/build.log" | sed 's/^/    /'
fi
result "$build_status"

# ---- 2. Tests ----
echo "[2/3] Running tests..."
if [ "$build_status" -ne 0 ]; then
    echo "  Skipped: build failed"
    result 1
else
    ctest --test-dir "$build_dir" --output-on-failure >"$log_dir/tests.log" 2>&1
    tests_status=$?
    # ctest summary: "100% tests passed out of 139" or
    #                "97% tests passed, 4 tests failed out of 139"
    summary=$(grep -E 'tests passed.*out of [0-9]+' "$log_dir/tests.log" | tail -n 1)
    if [ -n "$summary" ]; then
        total=$(echo "$summary" | sed 's/.*out of \([0-9]*\).*/\1/')
        tests_failed=$(echo "$summary" | sed -n 's/.*, \([0-9]*\) tests\{0,1\} failed.*/\1/p')
        passed=$((total - ${tests_failed:-0}))
        echo "  Tests Passed: $passed/$total"
    else
        echo "  Tests Passed: unknown (no ctest summary, see $log_dir/tests.log)"
        tests_status=1
    fi
    if [ "$tests_status" -ne 0 ]; then
        grep -E '^\s*[0-9]+ - ' "$log_dir/tests.log" | sed 's/^ */    failed: /'
    fi
    result "$tests_status"
fi

# ---- 3. Run ----
echo "[3/3] Running $build_dir/cyrus standalone for ${run_seconds}s..."
if [ "$build_status" -ne 0 ]; then
    echo "  Skipped: build failed"
    result 1
else
    # Prefix each Qt log line with its type so errors can be told apart from noise.
    QT_MESSAGE_PATTERN='%{type}: %{message}' \
        timeout "$run_seconds" "./$build_dir/cyrus" >"$log_dir/run.log" 2>&1
    run_exit=$?
    run_status=0
    # timeout exits 124 when it had to stop the app, i.e. it was still running.
    if [ "$run_exit" -eq 124 ]; then
        echo "  App still running after ${run_seconds}s"
    else
        echo "  App exited early (exit $run_exit)"
        run_status=1
    fi
    errors=$(grep -iE '^(critical|fatal):|error|segmentation fault|abort' "$log_dir/run.log")
    if [ -n "$errors" ]; then
        echo "  Errors logged:"
        echo "$errors" | sed 's/^/    /'
        run_status=1
    else
        echo "  No errors logged ($(grep -c '^warning:' "$log_dir/run.log") warnings, see $log_dir/run.log)"
    fi
    result "$run_status"
fi

if [ "$failed" -eq 0 ]; then echo "All checks passed."; else echo "Some checks failed."; fi
exit "$failed"
