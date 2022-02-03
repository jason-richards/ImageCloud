#pragma once

#include <memory>
#include <string>


namespace KERNEL {




//////////////////////////////////////////////////////
//  Stop Token
//////////////////////////////////////////////////////
class StopToken;
using StopTokenPtr = std::shared_ptr<StopToken>;


/* Create a new StopToken Pointer.
 *
 * @result StopTokenPtr - Pointer to a new StopToken object.
 */
StopTokenPtr
CreateStopToken();



/* Request an asynchronous stop.
 * @param stopper - Pointer to a StopToken object;
 */
void
RequestStop(
  StopTokenPtr stopper
);




//////////////////////////////////////////////////////
//  Kernel Context
//////////////////////////////////////////////////////
class Context;
using ContextPtr = std::shared_ptr<Context>;


/* Create a new Kernel Context Pointer.
 *
 * @param configYAML 	- Name of YAML Configuration file.
 * @result ContextPtr - Pointer to a new Kernel Context
 */
ContextPtr
CreateContext (
	const std::string& configYAML
);



/* Start the Kernel.
 * @param context - Pointer to Kernel Context
 * @param stopper - Pointer to a StopToken object for asynchronous control.
 * @result bool 	- True if successfully started Kernel.
 */
bool
Start (
	ContextPtr context,
  StopTokenPtr stopper
);



/* Obtain status of the Kernel.
 * @param context - Pointer to Kernel Context
 * @param string  - Return kernel status string
 */
void
Status (
	ContextPtr 		context,
	std::string& 	statusString
);


} // namespace KERNEL

