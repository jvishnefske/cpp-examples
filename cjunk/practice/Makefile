CXXFLAGS:=-std=c++2a -pthread
CXXFLAGS+=-fsanitize=undefined 
CXXFLAGS+=-Werror=all -Werror=extra
CXXFLAGS+=-Werror=return-type -Werror=unused-function -Werror=unused-variable
CXXFLAGS+=-I${HOME}/.local/include

all: chainablePromise sendFile isatty asyncStdin broken

broken: websocket-somethingsMIssing websocketBase UDPSocketServer quicServer geoParticleFilter antColonyOptimization
