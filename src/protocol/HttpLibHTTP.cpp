#include "HttpLibHTTP.hpp"
#include "utils/logger.hpp"

namespace geblaat {

int HttpLibHTTP::get(std::string url, int version) {
    auto uri = new uriparser::Uri(url);
    if (!uri->isValid()) {
        delete uri;
        uri = nullptr;
        return -1;
    }

    httplib::Client *client;

    int port = atoi(uri->port().c_str());
    if (uri->scheme() == "http") {
        if (!port)
            port = 80;

    } else if (uri->scheme() == "https") {
        if (!port)
            port = 443;

    } else {
        LOG_ERROR("Unsupported scheme %s", uri->scheme().c_str());
        delete uri;
        uri = nullptr;
        return -2;
    }

    client = new httplib::Client(uri->scheme() + "://" + uri->host() + ":" + std::to_string(port));

    if (!client) {
        delete uri;
        uri = nullptr;
        return -3;
    }

    httplib::Result res;
    std::string request = uri->path() + (uri->query().length() ? "?" : "") + uri->query();
    if (!request.length())
        request = "/";

    res = client->Get(request);

    if (res) {
        std::cout << res->status << std::endl;
        std::cout << res->get_header_value("Content-Type") << std::endl;
        std::cout << res->body << std::endl;
    } else {
        std::cout << "error code: " << res.error() << std::endl;

        auto result = client->get_openssl_verify_result();
        if (result) {
            std::cout << "verify error: " << X509_verify_cert_error_string(result) << std::endl;
        }
    }

    delete client;
    delete uri;

    return 0;
}

} // namespace geblaat

#ifdef DYNAMIC_LIBRARY
extern "C" {
geblaat::HttpLibHTTP *newInstance(void) { return new geblaat::HttpLibHTTP(); }
void delInstance(geblaat::HttpLibHTTP *inst) { delete inst; }
pluginloadable_t plugin_info = {
    .name = "HttpLibHTTP",
    .description = "HTTP Protocol support using cpp-httplib",
    .abi = {.abi = pluginloadable_abi_cpp, .version = 0},
};
}
#endif
