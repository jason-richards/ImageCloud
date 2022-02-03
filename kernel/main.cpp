#include <iostream>
#include <csignal>

#include "kernel.hpp"


KERNEL::StopTokenPtr stopper = KERNEL::CreateStopToken();


void
signalHandler(
  int signalNum
) {
  switch (signalNum) {
  case SIGINT:
    KERNEL::RequestStop(stopper);
  default:
    return;
  };
}


int
main(
	int 					argc,
	const char * 	argv []
) {

	if (argc < 2) {
		std::cerr << "Expected: " << argv[0] << " configuration.yaml" << std::endl;
		return -1;
	}

  signal(SIGINT, signalHandler);

	KERNEL::ContextPtr context = KERNEL::CreateContext(argv[1]);
	KERNEL::Start(context, stopper);

	return 0;
}

