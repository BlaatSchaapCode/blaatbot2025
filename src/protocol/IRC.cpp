/*
 * cIRC.cpp
 *
 *  Created on: 8 mrt. 2025
 *      Author: andre
 */

#include "IRC.hpp"

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

IRC::IRC() {
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

    mMessageParsers[Numeric::RPL_CREATED] = [this](const IRCMessage message) { onCreated(message); };
    mMessageParsers[Numeric::RPL_MYINFO] = [this](const IRCMessage message) { onMyInfo(message); };
    mMessageParsers[Numeric::RPL_ISUPPORT] = [this](const IRCMessage message) { onISupport(message); };

    mMessageParsers[Numeric::RPL_WELCOME] = [this](const IRCMessage message) { onWelcome(message); };
    mMessageParsers[Numeric::RPL_YOURHOST] = [this](const IRCMessage message) { onYourHost(message); };
}

IRC::~IRC() {
    // TODO Auto-generated destructor stub
    send("QUIT exited");
}

void IRC::onCanRegister(void) {
    if (mPass.length())
        send("PASS " + mPass);

    if (!mRealName.length())
        mRealName = mNick;

    send("USER " + mUser + " 0 * :" + mRealName);
    send("NICK " + mNick);
}

void IRC::onReady(void) {

    LOG_INFO("Ready");
    // To be called when the connection is ready.
    // called after either end of motd pr motd missing message.

    sendCTCPQuery("NickServ", "VERSION");

    if (serverInfo.features.count("BOT"))
        send("MODE " + mNick + " +" + serverInfo.features["BOT"]);

    send("JOIN #bscp-test");
}

void IRC::onConnected() {

    // Probe for capabilities
    // Note: when the server does not support capabilities it may response
    // with ERR_UNKNOWNCOMMAND (421)  but it is also possible it ignores the
    // message. We should start a timeout to handle such case
    send("CAP LS 302");

    // Probe for extensions
    send("MODE ISIRCX");
}

void IRC::onDisconnected() {}

void IRC::onUnknownCommand(const IRCMessage message) {
    if (message.command == Numeric::ERR_UNKNOWNCOMMAND) {
        if (!serverInfo.registrationComplete) {
            // Server replied unknown command to CAP LS
            // Continue with registration
            onCanRegister();
        }
    }
}

void IRC::onWelcome(const IRCMessage message) {
    serverInfo.registrationComplete = true;
    // "<client> :Welcome to the <networkname> IRC Network, <nick>[!<user>@<host>]"

    std::string preNetString = "Welcome to the ";
    std::string postNetString = " IRC Network";
    auto preNetPos = message.parameters[1].find(preNetString);
    auto postNetPos = message.parameters[1].find(postNetString);
    if (preNetPos != std::string::npos && postNetPos != std::string::npos) {
        serverInfo.network =
            message.parameters[1].substr(preNetString.length() + preNetPos, postNetPos - preNetPos - preNetString.length());
        LOG_INFO("Network : %s", serverInfo.network.c_str());
        std::string nickUserHost = message.parameters[1].substr(postNetString.length() + postNetPos + 1);
        LOG_INFO("NickUserHost : %s", nickUserHost.c_str());
    }
}

void IRC::onYourHost(const IRCMessage message) {
    // "<client> :Your host is <servername>, running version <version>"
    // All of this is also part of the MYINFO message
    // In a more machine-friendly manner

    //    std::string preHostStr = "Your host is ";
    //    std::string postHostStr = ", running version";
    //    auto preHostPos = message.parameters[1].find(preHostStr);
    //    auto postHostPos = message.parameters[1].find(postHostStr);
    //    if (preHostPos != std::string::npos && postHostPos != std::string::npos) {
    //        serverInfo.host =
    //            message.parameters[1].substr(preHostStr.length() + preHostPos, postHostPos - preHostPos - preHostStr.length());
    //        LOG_INFO("Host : %s", serverInfo.host.c_str());
    //
    //
    //		serverInfo.software = message.parameters[1].substr(postHostStr.length() + postHostPos + 1);
    //		LOG_INFO("Software : %s", serverInfo.software.c_str());
    //
    //    }
}

void IRC::onCreated(const IRCMessage message) {
    // "<client> :This server was created <datetime>"
}

void IRC::onMyInfo(const IRCMessage message) {
    //   "<client> <servername> <version> <available user modes>
    //   <available channel modes> [<channel modes with a parameter>]"

    serverInfo.host = message.parameters[1];
    serverInfo.software = message.parameters[2];

    serverInfo.userModes = message.parameters[3];
    serverInfo.channelModes = message.parameters[4];
    // Optional available
    serverInfo.channelModesWithParameters = message.parameters[5];

    LOG_INFO("Host : %s", serverInfo.host.c_str());
    LOG_INFO("IRCd : %s", serverInfo.software.c_str());
}
void IRC::onISupport(const IRCMessage message) {

    // https://defs.ircdocs.horse/defs/isupport.html
    const std::string iSupport = "are supported by this server";
    if (message.parameters.size() > 2) {
        if (message.parameters[message.parameters.size() - 1] == iSupport) {
            // Making sure Numeric 005 is RPL_ISUPPORT since there is a conflict
            // with RFC 2812  where 005 was RPL_BOUNCE instead
            // mServerISupport

            /*
             * Tokens of the form -PARAMETER are used to negate a previously
             * specified parameter. If the client receives a token like this,
             * the client MUST consider that parameter to be removed and revert
             * to the behaviour that would occur if the parameter was not
             * specified. The client MUST act as though the paramater is no
             * longer advertised to it. These tokens are intended to allow
             * servers to change their features without disconnecting clients.
             * Tokens of this form MUST NOT contain a value field.
             */
            std::vector<std::string> isupprt;
            isupprt.insert(isupprt.end(), message.parameters.begin() + 1, message.parameters.end() - 1);
            serverInfo.features.merge(parseKeyValue(isupprt));
            auto negations = parseNegation(isupprt);
            for (auto &negation : negations) {
                serverInfo.features.erase(negation);
            }

            return;
        }
    }
    /* RFC 2812 states
     005    RPL_BOUNCE "Try server <server name>, port <port number>"
     Is there any ancient ircd to test this against?
     Found some collection of ircds https://arsiv.behroozwolf.net/index.php?dir=servers/
     Also ftp://ftp.funet.fi/pub/unix/irc/server/
     */
}
void IRC::onCAP(const IRCMessage message) {
    if (message.command == "CAP") {
        serverInfo.hasCapabilities = true;
        if (message.parameters.size() > 2) {
            std::string subCommand = message.parameters[1];

            // TODO: Supported, Requested, Acknowledged capabilities

            if (subCommand == "LS") {
                bool moreCapabilitiesComing = message.parameters[2] == "*";
                serverInfo.capabilities.merge(parseKeyValue(splitString(message.parameters[2 + moreCapabilitiesComing])));

                if (!serverInfo.registrationComplete && !moreCapabilitiesComing) {
                    if (serverInfo.capabilities.count("message-tags"))
                        send("CAP REQ :message-tags");
                    send("CAP END");
                    onCanRegister();
                }
            }
        }
    }
}
void IRC::onIRCX(const IRCMessage message) {
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

void IRC::onPING(const IRCMessage message) {
    if (message.command == "PING") {
        if (message.parameters.size())
            send("PONG :" + message.parameters[0]);
    }
}

void IRC::onCTCPQuery(const IRCMessage message, const CTCPMessage ctcp) {
    if (ctcp.command == "ACTION") {
    } else if (ctcp.command == "CLIENTINFO") {
    } else if (ctcp.command == "DCC") {
    } else if (ctcp.command == "FINGER") {
    } else if (ctcp.command == "PING") {
        sendCTCPResponse(message.source.nick, ctcp.command, ctcp.parameters);
    } else if (ctcp.command == "SOURCE") {
        sendCTCPResponse(message.source.nick, "SOURCE", "https://github.com/BlaatSchaapCode/blaatbot2025/");
    } else if (ctcp.command == "TIME") {
        const auto zt{std::chrono::zoned_time{std::chrono::current_zone(), std::chrono::system_clock::now()}};
        sendCTCPResponse(message.source.nick, "TIME", std::format("{:%FT%T%z}", zt));
    } else if (ctcp.command == "VERSION") {
        utils::Version version;
        sendCTCPResponse(message.source.nick, "VERSION", "BlaatBot2025 " + version.m_git_commit);
    } else if (ctcp.command == "USERINFO") {
    }
}

void IRC::onCTCPResponse(const IRCMessage message, const CTCPMessage ctcp) {
    // TODO

    if (message.source.nick == "NickServ" && ctcp.command == "VERSION") {
        serverInfo.services = splitString(ctcp.parameters)[0];
        LOG_INFO("Services : %s", serverInfo.services.c_str());
    }
}

std::string IRC::stripFormatting(std::string formattedString) {
    // TODO strip all formatting
    // https://modern.ircdocs.horse/formatting

    std::string strippedString;

    enum code {
        bold = 0x02,
        color = 0x03,
        hexcolor = 0x04,
        reset = 0x0f,
        reversedcolor = 0x16,

        texticon = 0x1c,
        italics = 0x1d,
        underline = 0x1f,
        strikethrough = 0x1e,
        monospace = 0x11,
    };

    for (unsigned i = 0; i < formattedString.length(); i++) {

        switch ((unsigned char)formattedString[i]) {
        default:
            // Characters in the ASCII control range are used for
            // formatting and are stripped from the string
            // Special handling is only needed when they take parameters
            break;
        case 0x20 ... 0xFF:
            // Characters above the ASCII control range are considered text,
            // unless we are in a special case handling of parameters, below.
            strippedString += formattedString[i];
            break;
        case color:
            // needs special handling to consume its parameters
            /*
Forms of Color Codes

In the following list, <CODE> represents the color formatting character (0x03), <COLOR> represents one or two ASCII digits (either
0-9 or 00-99).

The use of this code can take on the following forms:

<CODE> - Reset foreground and background colors.
<CODE>, - Reset foreground and background colors and display the , character as text.
<CODE><COLOR> - Set the foreground color.
<CODE><COLOR>, - Set the foreground color and display the , character as text.
<CODE><COLOR>,<COLOR> - Set the foreground and background color.
*/
            // Eat the "CODE"
            if (i + 1 < formattedString.length())
                i++;

            // If digit, eat digit
            if (i + 1 < formattedString.length() && formattedString[i] >= '0' && formattedString[i] <= '9')
                i++;
            // If digit, eat digit
            if (i + 1 < formattedString.length() && formattedString[i] >= '0' && formattedString[i] <= '9')
                i++;
            // If comma, eat comma
            if (i + 1 < formattedString.length() && formattedString[i] == ',') {
                i++;
                // We are after a comma, we could eat two more digits.
                if (i + 1 < formattedString.length() && formattedString[i] >= '0' && formattedString[i] <= '9')
                    i++;
                if (i + 1 < formattedString.length() && formattedString[i] >= '0' && formattedString[i] <= '9')
                    i++;
            }
            // With above processing, we have eaten one byte too many, so give it back
            i--;

            break;
        case hexcolor:
            // needs special handling to consume its parameters
            /*
            Keep the Forms of Color Codes section above in mind, as this method of formatting keeps these same rules –
            the exceptions being that <CODE> represents the hex color character (0x04) and <COLOR> represents a
            six-digit hex value as RRGGBB.
            */
            // Length is fixed, we eat the next 6 bytes without further checking.
            if (i + 6 < formattedString.length())
                i += 6;
            break;
        case texticon:
            // needs special handling to consume its parameters
            // KvIRC extension, read till space
            if (formattedString.find(' ') != std::string::npos) {
                i += formattedString.find(' ');
            } else {
                // if there is no space, eg. the texticon appears at the
                // end of the string, we are done parsing, put i to the end
                i = formattedString.length();
            }
            break;
        }
    }

    return strippedString;
}

void IRC::onPRIVMSG(const IRCMessage message) {
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
        LOG_DEBUG("Message      : %s", privmsg.c_str());
        LOG_DEBUG("Clean Message: %s", cleanMessage.c_str());
    } else {
        // Malformed message?
    }
}
void IRC::onNOTICE(const IRCMessage message) {
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
                    if (ctcpString.length() >= parameterPos + 1)
                        ctcp.parameters = ctcpString.substr(parameterPos + 1);
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
void IRC::onERROR(const IRCMessage message) {}

void IRC::onNicknameInUse(const IRCMessage message) {
    if (!serverInfo.registrationComplete && Numeric::ERR_NICKNAMEINUSE == message.command) {
        // TODO, limit retries, make configurable
        mNick += "_";
        send("NICK " + mNick);
        return;
    }
}

void IRC::onMessage(const IRCMessage message) {

    if (mMessageParsers[message.command]) {
        mMessageParsers[message.command](message);
    } else {
        // Unknown message type
    }
}
void IRC::parseMessage(std::string line) {
    LOG_DEBUG((">>> " + line).c_str());
    IRCMessage message;
    message.raw = line;

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
            message.source.raw = tokens[0];
            message.source.raw.erase(0, 1); // remove the ':'
            tokens.erase(tokens.begin());

            auto userPos = message.source.raw.find("!");
            auto hostPos = message.source.raw.find("@");
            if (userPos != std::string::npos) {
                message.source.nick = message.source.raw.substr(0, userPos);
            } else {
                userPos = 0;
            }
            if (hostPos != std::string::npos) {
                message.source.user = message.source.raw.substr(userPos + 1, hostPos - userPos - 1);
                hostPos++;
            } else {
                hostPos = 0;
            }
            message.source.host = message.source.raw.substr(hostPos);
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

void IRC::onData(std::vector<char> data) {
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

void IRC::send(std::string message) {
    LOG_DEBUG(("<<< " + message).c_str());
    this->mConnection->send(message + "\r\n");
}

bool IRC::validTarget(const std::string target) {
    // TODO verify against specs what is valid
    if (target.find(" ") != std::string::npos)
        return false;
    if (target.find("\r") != std::string::npos)
        return false;
    if (target.find("\n") != std::string::npos)
        return false;

    return true;
}
bool IRC::validText(const std::string text) {
    // TODO verify against specs what is valid
    if (text.find("\r") != std::string::npos)
        return false;
    if (text.find("\n") != std::string::npos)
        return false;

    return true;
}

void IRC::sendPRIVMSG(const std::string target, const std::string text) {
    if (validTarget(target) && validText(text)) {
        send("PRIVMSG " + target + " :" + text);
    }
}
void IRC::sendNOTICE(const std::string target, const std::string text) {
    if (validTarget(target) && validText(text)) {
        send("NOTICE " + target + " :" + text);
    }
}
void IRC::sendCTCPQuery(const std::string target, const std::string command, const std::string parameters) {
    if (parameters.length())
        sendPRIVMSG(target, "\01" + command + " " + parameters + "\01");
    else
        sendPRIVMSG(target, "\01" + command + "\01");
}
void IRC::sendCTCPResponse(const std::string target, const std::string command, const std::string parameters) {
    if (parameters.length())
        sendNOTICE(target, "\01" + command + " " + parameters + "\01");
    else
        sendNOTICE(target, "\01" + command + "\01");
}

} // namespace protocol
