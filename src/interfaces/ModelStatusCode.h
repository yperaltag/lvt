#ifndef _LVT_MODELSTATUSCODE_H_
#define _LVT_MODELSTATUSCODE_H_

namespace lvt {

enum class ModelStatusCode {
    Ok,
    Already_Inserted,
    Not_Found,
    Internal_Error
};

} // namespace lvt

#endif // _LVT_MODELSTATUSCODE_H_
