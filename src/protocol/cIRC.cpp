/*
 * cIRC.cpp
 *
 *  Created on: 8 mrt. 2025
 *      Author: andre
 */

#include "cIRC.hpp"

#include "splitString.hpp"
#include <chrono>
#include <cstring>
#include <format>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

#include "logger.hpp"
#include "version.hpp"

namespace protocol {

/*
 * Channel names begin with
 * rfc1459: '&' or '#'
 * rfc2812: '&', '#', '+' or '!'
 * modern:  '&' or '#'
 *
 *  RFC 2811        Internet Relay Chat: Channel Management:
 *   Channels with '&' as prefix are local to the server where they are  created.
 *   Channels with '+' as prefix do not support channel modes.
 *   Channels with '!' as prefix are safe channels.
 *
 * Server lists supported channel types in CHANTYPES=# in "005"
 *
 */

cIRC::cIRC() {
    mMessageParsers["PING"] = [this](const IRCMessage message) { onPING(message); };
    mMessageParsers["PRIVMSG"] = [this](const IRCMessage message) { onPRIVMSG(message); };
    mMessageParsers["NOTICE"] = [this](const IRCMessage message) { onNOTICE(message); };
    mMessageParsers["ERROR"] = [this](const IRCMessage message) { onERROR(message); };
    mMessageParsers[Numeric::ERR_NICKNAMEINUSE] = [this](const IRCMessage message) { onNicknameInUse(message); };

    mMessageParsers[Numeric::RPL_ENDOFMOTD] = [this](const IRCMessage message) { onReady(); };
    mMessageParsers[Numeric::ERR_NOMOTD] = [this](const IRCMessage message) { onReady(); };

    mMessageParsers["CAP"] = [this](const IRCMessage message) { onCAP(message); };
    mMessageParsers[Numeric::IRCRPL_IRCX] = [this](const IRCMessage message) { onIRCX(message); };

    mMessageParsers[Numeric::ERR_UNKNOWNCOMMAND] = [this](const IRCMessage message) { onUnknownCommand(message); };

    // mMessageParsers[Numeric::RPL_WELCOME] = [this](const IRCMessage message) { onRegistrationMessage(message); };
    // mMessageParsers[Numeric::RPL_YOURHOST] = [this](const IRCMessage message) { onRegistrationMessage(message); };
    mMessageParsers[Numeric::RPL_CREATED] = [this](const IRCMessage message) { onISupport(message); };
    mMessageParsers[Numeric::RPL_MYINFO] = [this](const IRCMessage message) { onISupport(message); };
    mMessageParsers[Numeric::RPL_ISUPPORT] = [this](const IRCMessage message) { onISupport(message); };

    mMessageParsers[Numeric::RPL_WELCOME] = [this](const IRCMessage message) { onWelcome(message); };
    mMessageParsers[Numeric::RPL_YOURHOST] = [this](const IRCMessage message) { onYourHost(message); };
}

cIRC::~cIRC() {
    // TODO Auto-generated destructor stub
}

void cIRC::onCanRegister(void) {
    if (mPass.length())
        send("PASS " + mPass);

    if (!mRealName.length())
        mRealName = mNick;

    send("USER " + mUser + " 0 * :" + mRealName);
    send("NICK " + mNick);
}

void cIRC::onReady(void) {
    // To be called when the connection is ready.
    // called after either end of motd pr motd missing message.

    if (serverInfo.capabilities.count("message-tags"))
        send("CAP REQ :message-tags");

    if (serverInfo.features.count("BOT"))
        send("MODE " + mNick + " +" + serverInfo.features["BOT"]);

    send("JOIN #bscp-test");
}

void cIRC::onConnected() {

    // Probe for capabilities
    // Note: when the server does not support capabilities it may response
    // with ERR_UNKNOWNCOMMAND (421)  but it is also possible it ignores the
    // message. We should start a timeout to handle such case
	send("CAP LS 302");

    // Probe for extensions
    send("MODE ISIRCX");
}

void cIRC::onDisconnected() {}

void cIRC::onUnknownCommand(const IRCMessage message) {
    if (message.command == Numeric::ERR_UNKNOWNCOMMAND) {
        if (!serverInfo.registrationComplete) {
            // Server replied unknown command to CAP LS
            // Continue with registration
            onCanRegister();
        }
    }
}

void cIRC::onWelcome(const IRCMessage message) {
    serverInfo.registrationComplete = true;
    // "<client> :Welcome to the <networkname> IRC Network, <nick>[!<user>@<host>]"
}

void cIRC::onYourHost(const IRCMessage message) {
    // "<client> :Your host is <servername>, running version <version>"
}

void cIRC::onCreated(const IRCMessage message) {
    // "<client> :This server was created <datetime>"
}

void cIRC::onMyInfo(const IRCMessage message) {
    //   "<client> <servername> <version> <available user modes>
    //   <available channel modes> [<channel modes with a parameter>]"
}
void cIRC::onISupport(const IRCMessage message) {

    // https://defs.ircdocs.horse/defs/isupport.html
    const std::string iSupport = "are supported by this server";
    if (message.parameters.size() > 2) {
        if (message.parameters[message.parameters.size() - 1] == iSupport) {
            // Making sure Numeric 005 is RPL_ISUPPORT since there is a conflict
            // with RFC 2812  where 005 was RPL_BOUNCE instead
            // mServerISupport

            std::vector<std::string> isupprt;
            isupprt.insert(isupprt.end(), message.parameters.begin() + 1, message.parameters.end() - 1);
            serverInfo.features.merge(parseKeyValue(isupprt));
            return;
        }
    }
    /* RFC 2812 states
     005    RPL_BOUNCE "Try server <server name>, port <port number>"
     Is there any ancient ircd to test this against?
     */

}
void cIRC::onCAP(const IRCMessage message) {
    if (message.command == "CAP") {
        serverInfo.hasCapabilities = true;
        if (message.parameters.size() > 2) {
            std::string subCommand = message.parameters[1];
            if (subCommand == "LS") {
                bool moreCapabilitiesComing = message.parameters[2] == "*";
                serverInfo.capabilities.merge(parseKeyValue(splitString(message.parameters[2 + moreCapabilitiesComing])));

                if (!serverInfo.registrationComplete && !moreCapabilitiesComing) {
                    send("CAP END");
                    onCanRegister();
                }
            }
        }
    }
}
void cIRC::onIRCX(const IRCMessage message) {
    // >>> :irc.mysite.com 800 Anonymous 0 0 ANON 512 *
    // <state> <version> <package-list> <maxmsg> <option-list>
    serverInfo.hasExtensions = true;

    serverInfo.extensions.enabled = message.parameters[1] == "1";
    try {
        serverInfo.extensions.version = std::stoi(message.parameters[2]);
    } catch (...) {
        serverInfo.extensions.version = -1;
    }
    serverInfo.extensions.packages = splitString(message.parameters[3]);
    try {
        serverInfo.maxLen = std::stoi(message.parameters[4]); // TODO
    } catch (...) {
        serverInfo.maxLen = 512;
    }
    if (message.parameters[5] == "*") {
        serverInfo.extensions.options.clear();
    } else {
        serverInfo.extensions.options = splitString(message.parameters[5]);
    }
    // TODO: check if we already have tried enabling,
    // preventing an endless loop if the server supports IRCX but
    // denies our enable request.
    if (!serverInfo.extensions.enabled)
        send("IRCX");

    if (!serverInfo.registrationComplete && serverInfo.extensions.enabled)
        onCanRegister();
}

void cIRC::onPING(const IRCMessage message) {
    if (message.command == "PING") {
        if (message.parameters.size())
            send("PONG :" + message.parameters[0]);
    }
}

void cIRC::onCTCPQuery(const IRCMessage message, const CTCPMessage ctcp) {
    if (ctcp.command == "ACTION") {
    } else if (ctcp.command == "CLIENTINFO") {
    } else if (ctcp.command == "DCC") {
    } else if (ctcp.command == "FINGER") {
    } else if (ctcp.command == "PING") {
        sendCTCPResponse(message.nick, ctcp.command, ctcp.parameters);
    } else if (ctcp.command == "SOURCE") {
        sendCTCPResponse(message.nick, "SOURCE", "https://github.com/BlaatSchaapCode/blaatbot2025/");
    } else if (ctcp.command == "TIME") {
        // This prints in UTC
        // sendCTCPResponse(message.nick, "TIME", std::format("{:%FT%TZ}", std::chrono::system_clock::now()));

        //    	 adds the +0000 timezone... but how to get the timezone in the string

        const auto zt{std::chrono::zoned_time{std::chrono::current_zone(), std::chrono::system_clock::now()}};

        sendCTCPResponse(message.nick, "TIME", std::format("{:%FT%T%z}", zt));

    } else if (ctcp.command == "VERSION") {
        utils::Version version;
        sendCTCPResponse(message.nick, "VERSION", "BlaatBot2025 " + version.m_git_commit);
    } else if (ctcp.command == "USERINFO") {
    }
}

void cIRC::onCTCPResponse(const IRCMessage message, const CTCPMessage ctcp) {
    // TODO}
}

std::string cIRC::stripFormatting(std::string formattedString) {
    // TODO strip all formatting
    return formattedString;
}

void cIRC::onPRIVMSG(const IRCMessage message) {
    if (message.parameters.size() == 2) {
        std::string recipient = message.parameters[0];
        std::string privmsg = message.parameters[1];
        if (privmsg.length()) {
            if (privmsg[0] == 1) {
                // CTCP
                std::string ctcpString = privmsg.substr(1);
                // The last character is a CTCP message message should be
                // a 1 as well, but we should accept the message when it is
                // missing.
                if (ctcpString.length()) {
                    // making sure it has some length
                    if (ctcpString[ctcpString.length() - 1] == 1) {
                        // the last byte is 1 as well, as it should
                        ctcpString.erase(ctcpString.length() - 1, 1);
                    }
                }
                CTCPMessage ctcp;
                auto parameterPos = ctcpString.find(" ");
                if (parameterPos != std::string::npos) {
                    ctcp.command = ctcpString.substr(0, parameterPos);
                    if (ctcpString.length() >= parameterPos + 1)
                        ctcp.parameters = ctcpString.substr(parameterPos + 1);
                } else {
                    ctcp.command = ctcpString;
                }

                onCTCPQuery(message, ctcp);
                return;
            }
        }
        auto cleanMessage = stripFormatting(privmsg);
    } else {
        // Malformed message?
    }
}
void cIRC::onNOTICE(const IRCMessage message) {
    if (message.parameters.size() == 2) {
        std::string recipient = message.parameters[0];
        std::string notice = message.parameters[1];
        if (notice.length()) {
            if (notice[0] == 1) {
                // CTCP
                std::string ctcpString = notice.substr(1);
                // The last character is a CTCP message message should be
                // a 1 as well, but we should accept the message when it is
                // missing.
                if (ctcpString.length()) {
                    // making sure it has some length
                    if (ctcpString[ctcpString.length() - 1] == 1) {
                        // the last byte is 1 as well, as it should
                        ctcpString.erase(ctcpString.length() - 1, 1);
                    }
                }
                CTCPMessage ctcp;
                auto parameterPos = ctcpString.find(" ");
                if (parameterPos != std::string::npos) {
                    ctcp.command = ctcpString.substr(0, parameterPos);
                    if (ctcpString.length() >= parameterPos + 2)
                        ctcp.parameters = ctcpString.substr(parameterPos + 2);
                } else {
                    ctcp.command = ctcpString;
                }

                onCTCPResponse(message, ctcp);
                return;
            }
        }
        auto cleanMessage = stripFormatting(notice);
    } else {
        // Malformed message?
    }
}
void cIRC::onERROR(const IRCMessage message) {}

void cIRC::onNicknameInUse(const IRCMessage message) {
    if (!serverInfo.registrationComplete && Numeric::ERR_NICKNAMEINUSE == message.command) {
        // TODO, limit retries, make configurable
        mNick += "_";
        send("NICK " + mNick);
        return;
    }
}

void cIRC::onMessage(const IRCMessage message) {

    if (mMessageParsers[message.command]) {
        mMessageParsers[message.command](message);
    } else {
        // Unknown message type
    }
}
void cIRC::parseMessage(std::string line) {
    LOG_INFO((">>> " + line).c_str());
    IRCMessage message;

    std::string trailingParameter;

    auto trailingParameterPos = line.find(" :");
    if (trailingParameterPos != std::string::npos) {
        if (line[0] == '@') {
            // When tags are enabled, we might have a false positive
            // when a source parameter is available
            auto falsePositive = line.find(" ");
            if (falsePositive == trailingParameterPos) {
                auto lineWithoutPag = line.substr(trailingParameterPos + 2);
                trailingParameterPos += 2 + lineWithoutPag.find(" :");
            }
        }

        trailingParameter = line.substr(trailingParameterPos + 2);
        line = line.substr(0, trailingParameterPos);
    }

    auto tokens = splitString(line);

    // Tags, Optional, IRCv3
    // https://defs.ircdocs.horse/defs/tags
    if (tokens.size()) {
        if (tokens[0][0] == '@') {
            message.tags = tokens[0];
            message.tags.erase(0, 1); // remove the '@'
            tokens.erase(tokens.begin());
        }
    }

    // Source, Optional, RFC 1459
    if (tokens.size()) {
        if (tokens[0][0] == ':') {
            message.source = tokens[0];
            message.source.erase(0, 1); // remove the ':'
            tokens.erase(tokens.begin());

            auto userPos = message.source.find("!");
            auto hostPos = message.source.find("@");
            if (userPos != std::string::npos) {
                message.nick = message.source.substr(0, userPos);
            } else {
                userPos = 0;
            }
            if (hostPos != std::string::npos) {
                message.user = message.source.substr(userPos + 1, hostPos - userPos - 1);
            } else {
                hostPos = 0;
            }
            message.host = message.source.substr(hostPos + 1);
        }
    }

    // Command, RFC 1459
    if (tokens.size()) {
        message.command = tokens[0];
        tokens.erase(tokens.begin());
    } else {
        // Error, incorrectly formatted message
    }
    message.parameters = tokens;
    message.parameters.push_back(trailingParameter);
    onMessage(message);
}

void cIRC::onData(std::vector<char> data) {
    /*
     * IRCv3 "Modern IRC Client Protocol" states
     * When reading messages from a stream, read the incoming data into a
     * buffer. Only parse and process a message once you encounter the \r\n
     * at the end of it. If you encounter an empty message, silently ignore it.
     *
     * RFC 1459  and  RFC 2812 state
     * IRC messages are always lines of characters terminated with a CR-LF
     * (Carriage Return - Line Feed) pair, and these messages SHALL NOT
     * exceed 512 characters in length, counting all characters including
     * the trailing CR-LF. Thus, there are 510 characters maximum allowed
     * for the command and its parameters.  There is no provision for
     * continuation of message lines.
     *
     * IRCv3 "Modern IRC Client Protocol" states
     * Most IRC servers limit messages to 512 bytes in length, including
     * the trailing CR-LF characters. Implementations which include message
     * tags need to allow additional bytes for the tags section of a message;
     * clients must allow 8191 additional bytes and servers must allow 4096
     * additional bytes.
     */

    mBuffer.insert(mBuffer.end(), data.begin(), data.end());

    while (true) {
        std::string eolMarker = "\r\n";
        auto end = std::search(mBuffer.begin(), mBuffer.end(), eolMarker.begin(), eolMarker.end());
        if (end != mBuffer.end()) {
            std::string message = std::string(mBuffer.begin(), end);
            mBuffer.erase(mBuffer.begin(), end + 2);
            parseMessage(message);
        } else
            break;
    }
}

void cIRC::send(std::string message) {
    LOG_INFO(("<<< " + message).c_str());
    this->mConnection->send(message + "\r\n");
}

bool cIRC::validTarget(const std::string target) {
    // TODO verify against specs what is valid
    if (target.find(" ") != std::string::npos)
        return false;
    if (target.find("\r") != std::string::npos)
        return false;
    if (target.find("\n") != std::string::npos)
        return false;

    return true;
}
bool cIRC::validText(const std::string text) {
    // TODO verify against specs what is valid
    if (text.find("\r") != std::string::npos)
        return false;
    if (text.find("\n") != std::string::npos)
        return false;

    return true;
}

void cIRC::sendPRIVMSG(const std::string target, const std::string text) {
    if (validTarget(target) && validText(text)) {
        send("PRIVMSG " + target + " :" + text);
    }
}
void cIRC::sendNOTICE(const std::string target, const std::string text) {
    if (validTarget(target) && validText(text)) {
        send("NOTICE " + target + " :" + text);
    }
}
void cIRC::sendCTCPQuery(const std::string target, const std::string command, const std::string parameters) {
    if (parameters.length())
        sendPRIVMSG(target, "\01" + command + " " + parameters + "\01");
    else
        sendPRIVMSG(target, "\01" + command + "\01");
}
void cIRC::sendCTCPResponse(const std::string target, const std::string command, const std::string parameters) {
    if (parameters.length())
        sendNOTICE(target, "\01" + command + " " + parameters + "\01");
    else
        sendNOTICE(target, "\01" + command + "\01");
}

} // namespace protocol
