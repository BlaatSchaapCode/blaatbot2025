/*

 Author:	André van Schoubroeck <andre@blaatschaap.be>
 License:	MIT

 SPDX-License-Identifier: MIT

 Copyright (c) 2025 André van Schoubroeck <andre@blaatschaap.be>

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:
 The above copyright notice and this permission notice shall be included in all
 copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 SOFTWARE.

 */

#pragma once

#include <map>
#include <string>
#include <vector>

#include "BotModule.hpp"

namespace geblaat {

class TestBotModule : public BotModule {
  public:
    virtual ~TestBotModule();
    int setConfig(nlohmann::json) override;

  private:
    void onTest(std::string command, std::string parameters, std::map<std::string, std::string> message);
    void onRandomQuote(std::string command, std::string parameters, std::map<std::string, std::string> message);
    void onLoadQuotes(std::string command, std::string parameters, std::map<std::string, std::string> me);

    std::string quoteFileName;
    std::vector<std::string> mQuotes;
    std::string getRandomQuote(void);
    void loadQuotes(void);
};

} // namespace geblaat
