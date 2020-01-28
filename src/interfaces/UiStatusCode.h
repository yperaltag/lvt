#ifndef _LVT_UISTATUSCODE_H_
#define _LVT_UISTATUSCODE_H_

namespace lvt {

enum class UiStatusCode {
    Ok,
    Created,
    Bad_Request,
    Not_Found,
    Internal_Server_Error
};

} // namespace lvt

#endif // _LVT_UISTATUSCODE_H_
