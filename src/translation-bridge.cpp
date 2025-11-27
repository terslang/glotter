#include "translation-bridge.hpp"
#include "config.hpp"

#include <memory>
#include <sstream>
#include <string>
#include <unicode/translit.h>
#include <unicode/unistr.h>

namespace glotter {
TranslationBridge::TranslationBridge() {
  kotki = std::make_unique<Kotki>();
  kotki->scan(std::filesystem::path(MODELS_REGISTRY_FILE_PATH));

  UErrorCode status = U_ZERO_ERROR;
  transliterator.reset(icu::Transliterator::createInstance(
      "Any-Latin; Latin-ASCII", UTRANS_FORWARD, status));
  if (U_FAILURE(status) || !transliterator) {
    throw std::runtime_error("Failed to create transliterator");
  }

  tagger.reset(MeCab::createTagger(""));
  if (!tagger) {
    throw std::runtime_error("Failed to create MeCab tagger");
  }
}

std::string TranslationBridge::transliterate(const std::string &text,
                                             const std::string &lang) const {
  std::string input;
  if (lang == "ja") {
    input = kanji_to_katakana(text);
  } else {
    input = text;
  }

  icu::UnicodeString src = icu::UnicodeString::fromUTF8(input);
  transliterator->transliterate(src);
  std::string result;
  src.toUTF8String(result);
  return result;
}

std::string TranslationBridge::tarnslate(const std::string &text,
                                         const std::string &from,
                                         const std::string &to) const {
  std::string result = kotki->translate(text, from + to);
  if (result.empty()) {
    throw std::runtime_error("Translation failed");
  }

  return result;
}

std::string
TranslationBridge::kanji_to_katakana(const std::string &text) const {
  std::istringstream input_stream(text);
  std::string line;
  std::ostringstream katakana_stream;
  bool first_line = true;

  while (std::getline(input_stream, line)) {
    if (!first_line) {
      katakana_stream << '\n';
    }
    first_line = false;

    const MeCab::Node *node = tagger->parseToNode(line.c_str());

    for (; node; node = node->next) {
      if (node->feature) {
        std::string feature(node->feature);
        std::vector<std::string> parts;
        std::istringstream ss(feature);
        std::string token;

        while (std::getline(ss, token, ',')) {
          parts.push_back(token);
        }

        if (parts.size() > 8 && parts[8] != "*") {
          katakana_stream << parts[8] << " ";
        }
      }
    }
  }

  return katakana_stream.str();
}
} // namespace glotter
