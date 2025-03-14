/*
 * cIRC.cpp
 *
 *  Created on: 8 mrt. 2025
 *      Author: andre
 */

#include "cIRC.hpp"

#include "splitString.hpp"
#include <cstring>
#include <iostream>
#include <sstream>
#include <string>

#include "logger.hpp"

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

cIRC::cIRC(std::string nick, std::string user) {
    if (user.length())
        mUser = user;
    else
        mUser = nick;
    mNick = nick;

    mMessageParsers["PING"] = [this](const IRCMessage message) { onPING(message); };
    mMessageParsers["PRIVMSG"] = [this](const IRCMessage message) { onPRIVMSG(message); };
    mMessageParsers["NOTICE"] = [this](const IRCMessage message) { onNOTICE(message); };
    mMessageParsers["ERROR"] = [this](const IRCMessage message) { onERROR(message); };
}

cIRC::~cIRC() {
    // TODO Auto-generated destructor stub
}

void cIRC::onConnected() {

    mMessageParsers["CAP"] = [this](const IRCMessage message) { onMessageDuringRegistration(message); };
    mMessageParsers[Numeric::ERR_UNKNOWNCOMMAND] = [this](const IRCMessage message) { onMessageDuringRegistration(message); };

    mMessageParsers[Numeric::ERR_NICKNAMEINUSE] = [this](const IRCMessage message) { onMessageDuringRegistration(message); };

    send("CAP LS 302");
    // CAP either returns  capability list "CAP" or 421
    // << CAP LS 302
    // >> :eu3.chat4all.org 421 andre CAP :Unknown command
}

void cIRC::onDisconnected() {}

void cIRC::onMessageDuringRegistration(const IRCMessage message) {

    if (Numeric::ERR_NICKNAMEINUSE == message.command) {
        // TODO, limit retries, make configurable
        mNick += "_";
        send("NICK " + mNick);
        return;
    }

    if (message.command == Numeric::ERR_UNKNOWNCOMMAND) {
        LOG_INFO("Server does not support IRCv3 Capabilities");
    }

    if (message.command == "CAP") {
        LOG_INFO("Server supports IRCv3 capabilities");
        // TODO Parse capabilities, if when it contains message-tags, use it
        // TODO How investigate how SASL works

        if (message.parameters.size() > 2 && message.parameters[1] == "LS") {
            // Further CAP Messages will be handled by the onCAP handler
            mMessageParsers["CAP"] = [this](const IRCMessage message) { onCAP(message); };
            mServerSupportsCapabilities = true;

            // TODO Capabilities require further parsing of their parameters.
            mServerCapabilities = splitString(message.parameters[1]);
            if (std::find(mServerCapabilities.begin(), mServerCapabilities.end(), "message-tags") == mServerCapabilities.end()) {
                send("CAP REQ :message-tags");
            }
        }
    }

    if (mPass.length())
        send("PASS " + mPass);

    // TODO. Move the setting up user, pass, nick, realname around
    if (!mRealName.length())
        mRealName = mNick;

    send("USER " + mUser + " 0 * :" + mRealName);
    send("NICK " + mNick);
    if (mServerSupportsCapabilities)
        send("CAP END");

    mMessageParsers[Numeric::ERR_UNKNOWNCOMMAND] = nullptr;
    mMessageParsers[Numeric::ERR_NICKNAMEINUSE] = nullptr;
}
void cIRC::onCAP(const IRCMessage message) {}

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
        if (ctcp.parameters.length())
            sendCTCPResponse(message.nick, ctcp.command + " " + ctcp.parameters);
        else
            sendCTCPResponse(message.nick, ctcp.command);
    } else if (ctcp.command == "SOURCE") {
    } else if (ctcp.command == "TIME") {
    } else if (ctcp.command == "VERSION") {
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
void cIRC::sendCTCPQuery(const std::string target, const std::string text) { sendPRIVMSG(target, "\01" + text + "\01"); }
void cIRC::sendCTCPResponse(const std::string target, const std::string text) { sendNOTICE(target, "\01" + text + "\01"); }

} // namespace protocol
