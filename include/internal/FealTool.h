#ifndef _FEAL_TOOL_H
#define _FEAL_TOOL_H

#ifndef _FEAL_H
#error "Please include feal.h and not the other internal Feal header files, to avoid include errors."
#endif

namespace feal
{

class Tool
{
public:

Tool() = default;
virtual ~Tool() = default;

virtual void startTool(void){}
virtual void pauseTool(void){}
virtual void shutdownTool(void){}

};


} // namespace feal

#endif // _FEAL_TOOL_H
