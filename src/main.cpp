#include "config.hpp"
#include "cpp-httplib/httplib.h"
#include "translation-bridge.hpp"
#include <nlohmann/json.hpp>
#include <stdexcept>
#include <string>

using json = nlohmann::json;

#define JSON_CONTENT_TYPE "application/json"
#define PLAIN_TEXT_CONTENT_TYPE "text/plain"

int main() {
  glotter::TranslationBridge bridge;
  httplib::Server svr;

  svr.Get("/version", [&](const httplib::Request &req, httplib::Response &res) {
    res.set_content(GLOTTER_VERSION, PLAIN_TEXT_CONTENT_TYPE);
  });

  svr.Get("/source-code",
          [&](const httplib::Request &req, httplib::Response &res) {
            res.status = httplib::StatusCode::MovedPermanently_301;
            res.set_header("Location", "https://github.com/terslang/glotter");
          });

  svr.Get("/languages",
          [&](const httplib::Request &req, httplib::Response &res) {
            json j_array = json::array();
            for (const auto &lang : bridge.getSupportedLanguages()) {
              json j_lang;
              j_lang["code"] = lang.first;
              j_lang["name"] = lang.second;
              j_array.push_back(j_lang);
            }

            res.set_content(j_array.dump(), JSON_CONTENT_TYPE);
          });

  svr.Post(
      "/translate", [&](const httplib::Request &req, httplib::Response &res) {
        json j_body = json::parse(req.body);

        std::string from = j_body.contains("from") ? j_body["from"] : "";
        std::string to = j_body.contains("to") ? j_body["to"] : "";
        std::string text = j_body.contains("text") ? j_body["text"] : "";

        if (from.empty() || to.empty() || text.empty()) {
          res.status = httplib::StatusCode::BadRequest_400;
          res.set_content("'text', 'from' or 'to' parameter cannot be empty",
                          PLAIN_TEXT_CONTENT_TYPE);
          return;
        }

        json j_res;
        try {
          j_res["translation"] = bridge.tarnslate(text, from, to);
        } catch (const std::runtime_error) {
          res.status = httplib::StatusCode::InternalServerError_500;
          res.set_content("Translation failed", PLAIN_TEXT_CONTENT_TYPE);
          return;
        }

        res.set_content(j_res.dump(), JSON_CONTENT_TYPE);
      });

  svr.Post("/transliterate", [&](const httplib::Request &req,
                                 httplib::Response &res) {
    json j_body = json::parse(req.body);

    std::string lang = j_body.contains("lang") ? j_body["lang"] : "";
    std::string text = j_body.contains("text") ? j_body["text"] : "";

    if (lang.empty() || text.empty()) {
      res.status = httplib::StatusCode::BadRequest_400;
      res.set_content("'text' or 'lang' parameter cannot be empty",
                      PLAIN_TEXT_CONTENT_TYPE);
      return;
    }

    json j_res;
    try {
      j_res["transliteration"] = bridge.transliterate(text, lang);
      res.set_content(j_res.dump(), JSON_CONTENT_TYPE);
    } catch (const std::runtime_error) {
      res.status = httplib::StatusCode::InternalServerError_500;
      res.set_content("Transliteration failed", PLAIN_TEXT_CONTENT_TYPE);
    }
  });

  std::cout << "Server is running on http://localhost:9107" << std::endl;
  svr.listen("0.0.0.0", 9107);
}
