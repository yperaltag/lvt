#ifndef _LVT_UI_HTTP_URLPARAMETER_H_
#define _LVT_UI_HTTP_URLPARAMETER_H_

#include <string>

namespace lvt {
namespace ui {
namespace http {

struct UrlParameter {
    std::string name;
    std::string value;
};

} // namespace http
} // namespace ui
} // namespace lvt

#endif // _LVT_UI_HTTP_URLPARAMETER_H_
