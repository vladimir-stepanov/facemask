noinst_LTLIBRARIES = libgrapevine-statemachine.la

libgrapevine_statemachine_la_SOURCES = State.cpp StateTransition.cpp StateMachine.cpp

libgrapevine_statemachine_la_CPPFLAGS = -I$(top_srcdir)/include $(GRAPEVINE_CFLAGS)

library_includedir=$(includedir)/Grapevine/StateMachine
library_include_HEADERS = \
	../../include/Grapevine/StateMachine/State.hpp \
	../../include/Grapevine/StateMachine/StateTransition.hpp \
	../../include/Grapevine/StateMachine/StateMachine.hpp
