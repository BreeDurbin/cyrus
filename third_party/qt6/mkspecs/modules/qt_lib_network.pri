QT.network.VERSION = 6.7.3
QT.network.name = QtNetwork
QT.network.module = Qt6Network
QT.network.libs = $$QT_MODULE_LIB_BASE
QT.network.ldflags = 
QT.network.includes = $$QT_MODULE_INCLUDE_BASE $$QT_MODULE_INCLUDE_BASE/QtNetwork
QT.network.frameworks = 
QT.network.bins = $$QT_MODULE_BIN_BASE
QT.network.plugin_types = networkaccess networkinformation tls
QT.network.depends =  core
QT.network.uses = 
QT.network.module_config = v2
QT.network.DEFINES = QT_NETWORK_LIB
QT.network.enabled_features = http udpsocket networkproxy socks5 networkinterface networkdiskcache localserver dnslookup topleveldomain
QT.network.disabled_features = getifaddrs ipv6ifname securetransport schannel ssl dtls ocsp sctp brotli gssapi sspi
QT_CONFIG += http udpsocket networkproxy socks5 networkinterface networkdiskcache localserver dnslookup topleveldomain
QT_MODULES += network

