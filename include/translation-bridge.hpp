#pragma once

#include <array>
#include <kotki/kotki.h>
#include <mecab.h>
#include <string_view>
#include <unicode/translit.h>
#include <utility>

#define NO_OF_LANGS_SUPPORTED 51

namespace glotter {
class TranslationBridge {
public:
  TranslationBridge();
  ~TranslationBridge() = default;

  inline std::array<std::pair<std::string_view, std::string_view>,
                    NO_OF_LANGS_SUPPORTED>
  getSupportedLanguages() const {
    return lanaguages;
  }

  std::string tarnslate(const std::string &text, const std::string &from,
                        const std::string &to) const;
  std::string transliterate(const std::string &text,
                            const std::string &lang) const;
  bool is_non_latin_language(const std::string &lang) const;

private:
  static constexpr std::array<std::pair<std::string_view, std::string_view>,
                              NO_OF_LANGS_SUPPORTED>
      lanaguages{{{"ar", "Arabic"},     {"az", "Azerbaijani"},
                  {"be", "Belarusian"}, {"bg", "Bulgarian"},
                  {"bn", "Bengali"},    {"bs", "Bosnian"},
                  {"ca", "Catalan"},    {"cs", "Czech"},
                  {"da", "Danish"},     {"de", "German"},
                  {"el", "Greek"},      {"en", "English"},
                  {"es", "Spanish"},    {"et", "Estonian"},
                  {"fa", "Persian"},    {"fi", "Finnish"},
                  {"fr", "French"},     {"gu", "Gujarati"},
                  {"he", "Hebrew"},     {"hi", "Hindi"},
                  {"hr", "Croatian"},   {"hu", "Hungarian"},
                  {"id", "Indonesian"}, {"is", "Icelandic"},
                  {"it", "Italian"},    {"ja", "Japanese"},
                  {"kn", "Kannada"},    {"ko", "Korean"},
                  {"lt", "Lithuanian"}, {"lv", "Latvian"},
                  {"ml", "Malayalam"},  {"ms", "Malay"},
                  {"mt", "Maltese"},    {"nb", "Norwegian Bokmål"},
                  {"nl", "Dutch"},      {"nn", "Norwegian Nynorsk"},
                  {"pl", "Polish"},     {"pt", "Portuguese"},
                  {"ro", "Romanian"},   {"ru", "Russian"},
                  {"sk", "Slovak"},     {"sl", "Slovenian"},
                  {"sq", "Albanian"},   {"sr", "Serbian"},
                  {"sv", "Swedish"},    {"ta", "Tamil"},
                  {"te", "Telugu"},     {"tr", "Turkish"},
                  {"uk", "Ukrainian"},  {"vi", "Vietnamese"},
                  {"zh", "Chinese"}}};

  static constexpr std::array<std::string, 20> non_latin_langs{
      {"ar", "be", "bg", "bn", "el", "fa", "gu", "he", "hi", "ja",
       "kn", "ko", "ml", "mt", "ru", "sr", "ta", "te", "uk", "zh"}};

  std::unique_ptr<Kotki> kotki;
  std::unique_ptr<icu::Transliterator> transliterator;
  std::unique_ptr<MeCab::Tagger> tagger;
  std::string kanji_to_katakana(const std::string &text) const;
};
} // namespace glotter
