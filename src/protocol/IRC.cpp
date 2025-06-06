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

#include "IRC.hpp"

#include "splitString.hpp"
#include <algorithm>
#include <chrono>
#include <cstring>
#include <iomanip>
#include <iostream>
#include <random>
#include <sstream>
#include <string>

#ifdef ENABLE_ICU
// Note: Look at libunistring in stead of libicu
//       We don't really need it at the time
// IRC servers supporting UTF8MAPPING are rare
// Potentially make it optionally (another Module)
#include <unicode/normalizer2.h>
#include <unicode/translit.h>
#include <unicode/unistr.h>
#endif

#include "logger.hpp"
#include "time.hpp"
#include "timer.hpp"
#include "version.hpp"

#include "Client.hpp"
#include "PluginLoader.hpp"

namespace geblaat {

/*
 * Channel names begin with
 * rfc1459: '&' or '#'
 * rfc2812: '&', '#', '+' or '!'
 * modern:  '&' or '#'
 *
 *  RFC 2811        Internet Relay Chat: Channel Management:
 *   Channels with '&' as prefix are local to the server where they are created.
 *   Channels with '+' as prefix do not support channel modes.
 *   Channels with '!' as prefix are safe channels.
 *
 * Server lists supported channel types in CHANTYPES=# in "005"
 *
 *
 *
 */

IRC::IRC() {

    mMessageParsers["PONG"] = [this](IRCMessage &message) { onPONG(message); };
    mMessageParsers["PING"] = [this](IRCMessage &message) { onPING(message); };
    mMessageParsers["PRIVMSG"] = [this](IRCMessage &message) { onPRIVMSG(message); };
    mMessageParsers["TAGMSG"] = [this](IRCMessage &message) { onTAGMSG(message); };
    mMessageParsers["NOTICE"] = [this](IRCMessage &message) { onNOTICE(message); };
    mMessageParsers["ERROR"] = [this](IRCMessage &message) { onERROR(message); };
    mMessageParsers[Numeric::ERR_NICKNAMEINUSE] = [this](IRCMessage &message) { onNicknameInUse(message); };

    mMessageParsers[Numeric::RPL_ENDOFMOTD] = [this](IRCMessage &message) { onReady(); };
    mMessageParsers[Numeric::ERR_NOMOTD] = [this](IRCMessage &message) { onReady(); };

    mMessageParsers["CAP"] = [this](IRCMessage &message) { onCAP(message); };
    mMessageParsers[Numeric::IRCRPL_IRCX] = [this](IRCMessage &message) { onIRCX(message); };

    mMessageParsers[Numeric::ERR_UNKNOWNCOMMAND] = [this](IRCMessage &message) { onUnknownCommand(message); };

    mMessageParsers[Numeric::RPL_CREATED] = [this](IRCMessage &message) { onCreated(message); };
    mMessageParsers[Numeric::RPL_MYINFO] = [this](IRCMessage &message) { onMyInfo(message); };
    mMessageParsers[Numeric::RPL_ISUPPORT] = [this](IRCMessage &message) { onISupport(message); };

    mMessageParsers[Numeric::RPL_WELCOME] = [this](IRCMessage &message) { onWelcome(message); };
    mMessageParsers[Numeric::RPL_YOURHOST] = [this](IRCMessage &message) { onYourHost(message); };

    mMessageParsers["JOIN"] = [this](IRCMessage &message) { onJOIN(message); };
    mMessageParsers["MODE"] = [this](IRCMessage &message) { onMODE(message); };
    mMessageParsers[Numeric::RPL_CHANNELMODEIS] = [this](IRCMessage &message) { onChannelModeIs(message); };

    mMessageParsers[Numeric::RPL_TOPIC] = [this](IRCMessage &message) { onTopic(message); };
    mMessageParsers[Numeric::RPL_TOPICWHOTIME] = [this](IRCMessage &message) { onTopicWhoTime(message); };
    mMessageParsers[Numeric::RPL_NAMREPLY] = [this](IRCMessage &message) { onNamReply(message); };
    mMessageParsers[Numeric::RPL_ENDOFNAMES] = [this](IRCMessage &message) { onEndOfNames(message); };

    mMessageParsers[Numeric::RPL_WHOREPLY] = [this](IRCMessage &message) { onWhoReply(message); };
    mMessageParsers[Numeric::RPL_WHOSPCRPL] = [this](IRCMessage &message) { onWhoSpcReply(message); };
    mMessageParsers[Numeric::RPL_ENDOFWHO] = [this](IRCMessage &message) { onEndOfWho(message); };
    //
    //
}

IRC::~IRC() {
    // TODO Auto-generated destructor stub
    lagTimer.abortTimer();
    connectTimer.abortTimer();

    // TODO configurable quit message
    if (serverInfo.connected)
        send("QUIT exited");
}

int IRC::setConfig(const nlohmann::json &cfg) {
    try {
        config = cfg;

        if (config.contains("username") && config["username"].is_string()) {
            mUser = config["username"];
        } else {
            mUser = "geblaat";
        }

        if (config.contains("nickname") && config["nickname"].is_string()) {
            mNick = config["nickname"];
        } else {
            mNick = "geblaat";
        }

        if (config.contains("password") && config["password"].is_string()) {
            mPass = config["password"];
        } else {
            mPass = "";
        }

        if (config.contains("realname") && config["realname"].is_string()) {
            mRealName = config["realname"];
        } else {
            mRealName = "Geblaat (BlaatBot2025)";
        }

        if (config.contains("autojoin") && config["autojoin"].is_array()) {
            for (auto &join : config["autojoin"]) {
                AutoJoinChannel channel = {};
                if (join.contains("key") && join["key"].is_string()) {
                    channel.key = join["key"];
                }

                if (join.contains("channel") && join["channel"].is_string()) {
                    channel.channel = join["channel"];
                    autoJoinChannels.push_back(channel);
                }
            }
        }

        if (config.contains("connections") && config["connections"].is_array()) {
            // Initially we only support one connection defined, but we set up
            // the json to be able to handle multiple. If there are multiple
            // the default behaviour is intended to be to pick server at random
            auto jsonConnection = config["connections"][0];
            auto conn = pluginLoader->newInstance(jsonConnection["name"], "connection");
            mConnection = dynamic_cast<Connection *>(conn);

            if (mConnection) {
                mConnection->setConfig(jsonConnection["config"]);
                mConnection->setProtocol(this);
                mConnection->connect();
            }
        }
        //

    } catch (nlohmann::json::exception &ex) {
        LOG_ERROR("JSON exception: %s", ex.what());
        return -1;
    } catch (std::exception &ex) {
        LOG_ERROR("Unknown exception: %s", ex.what());
        return -1;
    } catch (...) {
        LOG_ERROR("Unknown exception (not derived from std::exception)");
        return -1;
    }

    return 0;
}

std::string IRC::toLower(std::string s) {
    /*
     The value MUST be specified and is a string. Servers MAY advertise
     alternate casemappings to those above, but clients MAY NOT be able to
     understand or perform them. If the parameter is not published by the
     server at all, clients SHOULD assume CASEMAPPING=rfc1459.
     */
    std::string caseMapping = "rfc1459";
    if (serverInfo.features.count("CASEMAPPING"))
        caseMapping = serverInfo.features["CASEMAPPING"];

    bool utf8Mapping = false;
    if (serverInfo.features.count("UTF8MAPPING")) {

        LOG_DEBUG("UTF8MAPPING: %s", serverInfo.features["UTF8MAPPING"].c_str());

        // See also
        // https://github.com/DanielOaks/ircv3-specifications/blob/master+rfc7700/documentation/rfc8265.md
        // suggesting

        // CASEMAPPING=ascii UTF8MAPPING=rfc8265

        // https://github.com/ircv3/ircv3-specifications/pull/272/commits/807e084fba9634084054def193424b9589f2a56b
        // https://github.com/DanielOaks/ircv3-specifications/blob/master%2Brfc7700/documentation/rfc8265.md

        // Changed casemapping from "rfc7700" to "rfc7613", to match new draft spec.
        // The recommended value of `server.casemapping` is now `ascii` instead of
        // `precis`.

        // rfc7613 mentioned in https://modern.ircdocs.horse/
        // rfc3454 mentioned in https://defs.ircdocs.horse/defs/isupport

        // Many possible values for utf8 mapping, for now, we only have one
        // casemapping case for utf8
        utf8Mapping = true;
    }

    // As some point, it was decided, when UTF8 case mapping is in use, rather
    // then putting some RFC in the casemapping, put ascii and create a new
    // UTF8MAPPING feature. However, older specs mention some values in
    // casemapping to indicate UTF8 mapping
    // *  rfc7613 mentioned in https://modern.ircdocs.horse/
    // *  rfc3454 mentioned in https://defs.ircdocs.horse/defs/isupport
    // If we encounter such, we use the, to my knowledge, latest approach:
    // First apply ASCII mapping, then apply UTF mapping.
    if ((caseMapping == "rfc3454") || (caseMapping == "rfc7700") || (caseMapping == "rfc7613") || (caseMapping == "rfc8265") ||
        (caseMapping == "precis") || (caseMapping == "utf8")) {
        caseMapping = "ascii";
        utf8Mapping = true;
    }

    if (caseMapping == "ascii") {
        // ascii: Defines the characters a to z to be considered the lower-case
        // equivalents of the characters A to Z only.

        // https://datatracker.ietf.org/doc/html/draft-hardy-irc-isupport-00#section-4.1
        // "ascii": The ASCII characters 97 to 122 (decimal) are defined as
        // the lower-case characters of ASCII 65 to 90 (decimal).  No other
        // character equivalency is defined.

        for (unsigned int i = 0; i < s.length(); i++) {
            if (s[i] >= 65 && s[i] <= 90)
                s[i] += 32;
        }

    } else if (caseMapping == "rfc1459") {
        // rfc1459: Same as 'ascii', with the addition of the characters
        // '{', '}', '|', and '^' being considered the lower-case equivalents
        // of the characters '[', ']', '\', and '~' respectively.

        // https://datatracker.ietf.org/doc/html/draft-hardy-irc-isupport-00#section-4.1
        // "rfc1459": The ASCII characters 97 to 126 (decimal) are defined as
        // the lower-case characters of ASCII 65 to 94 (decimal).  No other
        // character equivalency is defined.

        for (unsigned int i = 0; i < s.length(); i++) {
            if (s[i] >= 65 && s[i] <= 94)
                s[i] += 32;
        }

    } else if (caseMapping == "rfc1459-strict") {
        // rfc1459-strict: Same casemapping as 'ascii', with the characters
        // '{', '}', and '|' being the lower-case equivalents of
        // '[', ']', and '\', respectively. Note that the difference between
        // this and rfc1459 above is that in rfc1459-strict, '^' and '~' are not
        // casefolded.

        // https://datatracker.ietf.org/doc/html/draft-hardy-irc-isupport-00#section-4.1
        // "strict-rfc1459": The ASCII characters 97 to 125 (decimal) are
        // defined as the lower-case characters of ASCII 65 to 93 (decimal).
        // no other character equivalency is defined.

        for (unsigned int i = 0; i < s.length(); i++) {
            if (s[i] >= 65 && s[i] <= 93)
                s[i] += 32;
        }

    } else {
        // Unknown case mapping? What should we do in this case?
        // Safe to assume at least ASCII should map?
        for (unsigned int i = 0; i < s.length(); i++) {
            if (s[i] >= 65 && s[i] <= 90)
                s[i] += 32;
        }
    }
#ifdef ENABLE_ICU
    if (utf8Mapping) {
        // UTF8 case mapping

        // rfc7613 mentions "Unicode Default Case Folding as defined in
        // the Unicode Standard", which appears to be implemented by the ICU library
        // Should we use that library?

        // Please note: There appears to be StringPrep support
        // which is RFC 3454.
        // RFC 7564 (PRECIS) obsoletes RFC 3454.
        // However, if we got an RFC 3454 implementation,
        // RFC 8265 (Obsoletes 7613) mentions previous approach was
        // SASLPrep (RFC 4013)
        // Seems it does the BiDi thing? But preserves case?

        // https://datatracker.ietf.org/doc/html/rfc8265#section-3.3

        // However, I think the most important parts are covered, for now.

        // Convert string to icu's datatype
        icu::UnicodeString u;
        u.fromUTF8(s);

        //    1.  Width Mapping Rule: Map fullwidth and halfwidth code points to
        //        their decomposition mappings (see Unicode Standard Annex #11
        //        [UAX11]).

        // I have trouble finding the calls into ICU for the first step.
        // Case mapping and normalisation, that looks ok

        //    2.  Additional Mapping Rule: There is no additional mapping rule.

        // Trivial ;)

        //    3.  Case Mapping Rule: Map uppercase and titlecase code points to
        //        their lowercase equivalents, preferably using the Unicode
        //        toLowerCase() operation as defined in the Unicode Standard
        //        [Unicode]; see further discussion in Section 3.2.

        u.toLower();

        //    4.  Normalization Rule: Apply Unicode Normalization Form C (NFC) to
        //        all strings.

        UErrorCode errorCode;
        auto normalizer = icu::Normalizer2::getNFCInstance(errorCode);
        // TODO error checking
        u = normalizer->normalize(u, errorCode);
        // TODO error checking

        //    5.  Directionality Rule: Apply the "Bidi Rule" defined in [RFC5893]
        //        to strings that contain right-to-left code points (i.e., each of
        //        the six conditions of the Bidi Rule must be satisfied); for
        //        strings that do not contain right-to-left code points, there is
        //        no special processing for directionality.

        // There is some bidi functionality in the ICU library, but I do not
        // understand how any of that maps to any of the rules.

        // Processing done: Convert it back to a std::string
        u.toUTF8String(s);
    }
#else
    if (utf8Mapping) {
        LOG_INFO("UTF8 Mapping has been disabled");
    }
#endif
    return s;
}

bool IRC::isEqual(const std::string first, const std::string seccond) {

    // If the length differ, then they are not equal, we don't have to
    // look further.
    if (first.length() != seccond.length())
        return false;
    return toLower(first) == toLower(seccond);
}
void IRC::onCanRegister(void) {

    if (mPass.length())
        send("PASS " + mPass);

    if (!mRealName.length())
        mRealName = mNick;

    send("USER " + mUser + " 0 * :" + mRealName);
    send("NICK " + mNick);
}

void IRC::applyServerQuirks(void) {
    // Here we apply server quirks.

    // Please note: development is mainly done against
    // UnrealIRCd-6.1.9.1 and Anope-2.0.17

    //------------------------------------------------------------------------
    // IRCd
    //------------------------------------------------------------------------
    // IRC Daemon quirck. Determine the Daemon type, so we can handle daemon-
    // specific behaviours. Eg. supporing daemon specific commands and
    // responses, and adding features not being advertised but known supported
    // (mostly old versions of a daemon)
    //------------------------------------------------------------------------

    serverInfo.daemonFamily = "Other";
    //------------------------------------------------------------------------
    // UnrealIRCd
    //------------------------------------------------------------------------
    // TODO: Unreal3 does not advertise the Bot Mode support
    // Unreal6 does, but what about 4 and 5
    // Also note the string changed slightly, for version 3 we might get
    // "Unreal3.2.9" while version 6 says "UnrealIRCd-6.1.9.1"
    if (serverInfo.daemon.find("Unreal") == 0) {
        serverInfo.daemonFamily = "Unreal";
    }
    if (serverInfo.daemon.find("Unreal3") == 0) {
        // Running Unreal3.x.x
        // This version of Unreal supports Bot mode but does not advertise it
        // in its ISUPPORT messages. We inject it here.
        serverInfo.features["BOT"] = "B";
    }

    //------------------------------------------------------------------------
    // InspIRCd
    //------------------------------------------------------------------------
    // Observed string "InspIRCd-4"
    if (serverInfo.daemon.find("InspIRCd") == 0) {
        serverInfo.daemonFamily = "InspIRCd";
    }

    //------------------------------------------------------------------------
    // Services
    //------------------------------------------------------------------------
    // Services quircks. Determine the Services type, so we can handle
    // services-specific behaviours. Services have some variations around them.
    // For the time being we only consider NickServ/ChanServ style services.
    // We send a CTCP VERSION to NickServ to attempt to detect services.
    // If we ever wish to create an IRC client supporting handling services,
    // eg. supporting auto-ghosting, we need to know how to handle this.
    // Some services may use GHOST (Anope 1.x) while others use
    // RECOVER (Anope 2.x) for this.
    //------------------------------------------------------------------------

    // Anope 2.x (Anope-2.0.17)
    // Atheme  (atheme-7.3.0-rc2)

    // Anope 1.x (probably) (Services on Chat4All don't support CTCP VERSION)
}

void IRC::applyFeatures() {
    auto channelModes = splitString(serverInfo.features["CHANMODES"], ",");
    serverInfo.channelTypeAModes = channelModes[0];
    serverInfo.channelTypeBModes = channelModes[1];
    serverInfo.channelTypeCModes = channelModes[2];
    serverInfo.channelTypeDModes = channelModes[3];

    auto prefix = serverInfo.features["PREFIX"];

    // We have to parse a string like "(qaohv)~&@%+"

    auto haakjeopen = prefix.find("(");
    auto haakjesluit = prefix.find(")");
    if (haakjeopen != 0)
        return; // Error: Expect '(' at position 0,
    if (haakjesluit == std::string::npos)
        return; // ')' not found
    auto len = haakjesluit - 1;
    std::string modes, prefixes;
    modes = prefix.substr(1, len);
    if (prefix.length() == (2 * len) + 2) {
        prefixes = prefix.substr(len + 2);
        for (unsigned i = 0; i < len; i++) {
            serverInfo.channelMembershipPrefixes[modes[i]] = prefixes[i];
        }
    }
}

void IRC::onReady(void) {
    applyServerQuirks();
    applyFeatures();

    LOG_INFO("Ready");
    // To be called when the connection is ready.
    // called after either end of motd or motd missing message.

    sendCTCPQuery("NickServ", "VERSION");

    if (serverInfo.features.count("BOT"))
        send("MODE " + mNick + " +" + serverInfo.features["BOT"]);

    ping();

    for (auto &join : autoJoinChannels) {
        // TODO: a join function
        // TODO: merge multiple joins in one request
        send("JOIN " + join.channel + " " + join.key);
    }
}

void IRC::onConnected() {
    serverInfo.connected = true;

    // Set defaults
    serverInfo.hasCapabilities = false;
    serverInfo.capabilities.acknowledged.clear();
    serverInfo.capabilities.supported.clear();

    serverInfo.hasExtensions = false;
    serverInfo.features.clear();
    serverInfo.features["CASEMAPPING"] = "rfc1459";
    serverInfo.features["CHANTYPES"] = "#&";
    serverInfo.features["MODES"] = "3";
    serverInfo.features["PREFIX"] = "(ov)@+";

    // Probe for capabilities
    // Note: when the server does not support capabilities it may response
    // with ERR_UNKNOWNCOMMAND (421)  but it is also possible it ignores the
    // message. We should start a timeout to handle such case
    send("CAP LS 302");

    // Probe for extensions
    send("MODE ISIRCX");

    // Is there any server that implements both capabilities and extensions?
    // If that were the case the registration procedure may need to be a little
    // more complicated, as then we need to synchronise the probes, and then
    // decide what the way to continue the registration is. Especially, when
    // SASL is needed, as there are both capabilities and extensions describing
    // a SASL login procedure. Note that capabilities is the common method.
    // I haven't found an IRCX server implementing SASL extensions to test
    // against. Note: OfficeIRC, a commercial product, https://www.officeirc.com/
    // claims "New IRCv3 features, improved IRCX support and server linking
    // support." So it could be possible an implementation supports both.

    // If a server supports neither, we have a timeout.
    connectTimer.afterSeconds([this]() { onCanRegister(); }, std::chrono::seconds(3));
}

void IRC::onDisconnected() { serverInfo.connected = false; }

void IRC::onUnknownCommand(IRCMessage &message) {
    //    if (message.command == Numeric::ERR_UNKNOWNCOMMAND) {
    //        if (!serverInfo.registrationComplete) {
    //            // Server replied unknown command to CAP LS
    //            // Continue with registration
    //            onCanRegister();
    //        }
    //    }
}

void IRC::onWelcome(IRCMessage &message) {
    serverInfo.registrationComplete = true;
    // "<client> :Welcome to the <networkname> IRC Network,
    // <nick>[!<user>@<host>]"
    if (message.parameters.size() > 0) {

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
}

void IRC::onYourHost(IRCMessage &message) {
    // "<client> :Your host is <servername>, running version <version>"
    // All of this is also part of the MYINFO message
    // In a more machine-friendly manner

    //    std::string preHostStr = "Your host is ";
    //    std::string postHostStr = ", running version";
    //    auto preHostPos = message.parameters[1].find(preHostStr);
    //    auto postHostPos = message.parameters[1].find(postHostStr);
    //    if (preHostPos != std::string::npos && postHostPos != std::string::npos)
    //    {
    //        serverInfo.host =
    //            message.parameters[1].substr(preHostStr.length() + preHostPos,
    //            postHostPos - preHostPos - preHostStr.length());
    //        LOG_INFO("Host : %s", serverInfo.host.c_str());
    //
    //
    //		serverInfo.software =
    // message.parameters[1].substr(postHostStr.length() + postHostPos + 1);
    //		LOG_INFO("Software : %s", serverInfo.daemon.c_str());
    //
    //    }
}

void IRC::onCreated(IRCMessage &message) {
    // "<client> :This server was created <datetime>"
}

void IRC::onMyInfo(IRCMessage &message) {
    //   "<client> <servername> <version> <available user modes>
    //   <available channel modes> [<channel modes with a parameter>]"

    if (message.parameters.size() > 1)
        serverInfo.host = message.parameters[1];

    if (message.parameters.size() > 2)
        serverInfo.daemon = message.parameters[2];

    // Clients SHOULD discover available features using RPL_ISUPPORT
    // tokens rather than the mode letters listed in this reply.
    // There is CHANMODES in ISUPPORT, but I don't see a USERMODES
    // Thus I suppose we should look at the usermodes advertised here

    if (message.parameters.size() > 3)
        serverInfo.userModes = message.parameters[3];

    if (message.parameters.size() > 4)
        serverInfo.channelModes = message.parameters[4];
    // Optional available
    if (message.parameters.size() > 5)
        serverInfo.channelModesWithParameters = message.parameters[5];

    LOG_INFO("Host : %s", serverInfo.host.c_str());
    LOG_INFO("IRCd : %s", serverInfo.daemon.c_str());
}
void IRC::onISupport(IRCMessage &message) {

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

            // Merge doesn't replace the new values
            // serverInfo.features.merge(parseKeyValue(isupprt));

            for (auto &feature : parseKeyValue(isupprt)) {
                serverInfo.features[feature.first] = feature.second;
            }

            auto negations = parseNegation(isupprt);
            for (auto &negation : negations) {
                serverInfo.features.erase(negation);
                // TODO: any processing for negated features.
                // TODO: in what case such negation would occur?
            }

            return;
        }
    }
    /* RFC 2812 states
     005    RPL_BOUNCE "Try server <server name>, port <port number>"
     Is there any ancient ircd to test this against?
     Found some collection of ircds
     https://arsiv.behroozwolf.net/index.php?dir=servers/ Also
     ftp://ftp.funet.fi/pub/unix/irc/server/
     */
}

void IRC::requestCapabilityIfPresent(const std::string &cap) {
    if (serverInfo.capabilities.supported.contains(cap))
        send("CAP REQ :" + cap);
}

void IRC::onCAP(IRCMessage &message) {
    if (message.command == "CAP") {
        serverInfo.hasCapabilities = true;
        if (message.parameters.size() > 2) {
            std::string subCommand = message.parameters[1];

            // TODO: Supported, Requested, Acknowledged capabilities

            if (subCommand == "LS") {
                bool moreCapabilitiesComing = message.parameters[2] == "*";
                if (message.parameters.size() > (2u + moreCapabilitiesComing)) {
                    auto capabilities = splitString(message.parameters[2 + moreCapabilitiesComing]);
                    serverInfo.capabilities.supported.merge(parseKeyValue(capabilities));

                    auto negations = parseNegation(capabilities);
                    for (auto &negation : negations) {
                        serverInfo.capabilities.supported.erase(negation);
                        // TODO: any processing for negated capabilities?
                    }

                    if (!serverInfo.registrationComplete && !moreCapabilitiesComing) {

                        requestCapabilityIfPresent("message-tags");
                        requestCapabilityIfPresent("echo-message");
                        requestCapabilityIfPresent("batch");
                        requestCapabilityIfPresent("label");
                        requestCapabilityIfPresent("draft/labeled-response");
                        requestCapabilityIfPresent("account-notify");
                        requestCapabilityIfPresent("extended-join");
                        requestCapabilityIfPresent("away-notify");

                        send("CAP END");
                        connectTimer.abortTimer();
                        onCanRegister();
                    }
                }
            }

            if (subCommand == "ACK") {
                // serverInfo.capabilities.acknowledged.merge
                auto capabilities = splitString(message.parameters[2]);
                for (auto &capability : capabilities) {
                    if (capability.length()) {
                        if (capability[0] == '-') {
                            capability = capability.substr(1);
                            serverInfo.capabilities.acknowledged.erase(capability);

                            LOG_INFO("Removing %s from acknowlegded capabilities", capability.c_str());
                        } else {
                            serverInfo.capabilities.acknowledged.insert(capability);
                            LOG_INFO("Adding %s to acknowlegded capabilities", capability.c_str());
                        }
                    }
                }
            }

            if (subCommand == "NEW") {
                auto capabilities = splitString(message.parameters[2]);
                serverInfo.capabilities.supported.merge(parseKeyValue(capabilities));
                LOG_INFO("Adding %s to supported capabilities", message.parameters[2].c_str());
            }

            if (subCommand == "DEL") {
                auto capabilities = splitString(message.parameters[2]);
                for (auto &capability : capabilities) {
                    serverInfo.capabilities.supported.erase(capability);
                    LOG_INFO("Removing %s from supported capabilities", capability.c_str());
                    serverInfo.capabilities.acknowledged.erase(capability);
                }
            }

            if (subCommand == "LIST") {
                // query of acknowledged capabilities
            }
        }
    }
}
void IRC::onIRCX(IRCMessage &message) {
    // >>> :irc.mysite.com 800 Anonymous 0 0 ANON 512 *
    // <state> <version> <package-list> <maxmsg> <option-list>
    if (message.parameters.size() > 5) {

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

        if (!serverInfo.registrationComplete && serverInfo.extensions.enabled) {
            connectTimer.abortTimer();
            onCanRegister();
        }
    }
}

void IRC::onPING(IRCMessage &message) {
    if (message.parameters.size() > 0) {
        if (message.command == "PING") {
            if (message.parameters.size())
                send("PONG :" + message.parameters[0]);
        }
    }
}

void IRC::ping() {
    auto now = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch());
    send("PING :" + std::to_string(now.count()));
}

void IRC::onPONG(IRCMessage &message) {
    int pingInterval = 30;
    if (message.parameters.size() > 1) {
        std::chrono::milliseconds lag;
        if (message.parameters.size()) {
            try {
                auto sent_int = std::stoll(message.parameters[1]);
                auto sent = std::chrono::milliseconds(sent_int);
                auto now =
                    std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch());
                lag = now - sent;
                LOG_DEBUG("Lag is %d ms", (int)lag.count());
                serverInfo.lag = lag;
            } catch (...) {
                LOG_DEBUG("Unable to determine lag (parameter not integer)");
                serverInfo.lag = std::chrono::milliseconds::max();
                pingInterval = 60;
            }
        }
    } else {
        LOG_DEBUG("Unable to determine lag (missing parameter)");
        serverInfo.lag = std::chrono::milliseconds::max();
        pingInterval = 60;
    }
    lagTimer.afterSeconds([this]() { ping(); }, std::chrono::seconds(pingInterval));
}

void IRC::onCTCPQuery(IRCMessage &message, CTCPMessage &ctcp) {
    if (ctcp.command == "ACTION") {

        auto recipient = message.parameters[0];
        auto action = ctcp.parameters;
        std::map<std::string, std::string> m = messageToClient(message);
        m["type"] = "action";
        m["text/plain"] = stripFormatting(action);
        m["text/irc"] = action;
        if (mClient)
            mClient->onMessage(m);

    } else if (ctcp.command == "CLIENTINFO") {
    } else if (ctcp.command == "DCC") {
    } else if (ctcp.command == "FINGER") {
    } else if (ctcp.command == "PING") {
        sendCTCPResponse(message.source.nick, ctcp.command, ctcp.parameters);
    } else if (ctcp.command == "SOURCE") {
        sendCTCPResponse(message.source.nick, "SOURCE", "https://github.com/BlaatSchaapCode/blaatbot2025/");
    } else if (ctcp.command == "TIME") {
        sendCTCPResponse(message.source.nick, "TIME", getTimeString());
    } else if (ctcp.command == "VERSION") {
        utils::Version version;
        sendCTCPResponse(message.source.nick, "VERSION", "BlaatBot2025 " + version.m_git_commit);
    } else if (ctcp.command == "USERINFO") {
    }
}

void IRC::onCTCPResponse(IRCMessage &message, CTCPMessage &ctcp) {
    // TODO

    if (message.source.nick == "NickServ" && ctcp.command == "VERSION") {
        // Note... we might need to analyse some services
        // ergo responds "NickServ (ergo-v2.15.0)"
        if (message.parameters.size() > 0) {
            if (isEqual(splitString(ctcp.parameters)[0], "NickServ")) {
                if (message.parameters.size() > 1)
                    serverInfo.services = splitString(ctcp.parameters)[1];
            } else {
                serverInfo.services = splitString(ctcp.parameters)[0];
            }

            if (serverInfo.services.length()) {
                if (serverInfo.services[0] == '(')
                    serverInfo.services.erase(0, 1);
                if (serverInfo.services[serverInfo.services.length() - 1] == ')')
                    serverInfo.services.erase(serverInfo.services.length() - 1, 1);
            }
            LOG_INFO("Services : %s", serverInfo.services.c_str());
        }
    }
}

std::string IRC::stripFormatting(const std::string &formattedString) {
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

      In the following list, <CODE> represents the color formatting character (0x03),
      <COLOR> represents one or two ASCII digits (either 0-9 or 00-99).

      The use of this code can take on the following forms:

      <CODE> - Reset foreground and background colors.
      <CODE>, - Reset foreground and background colors and display the , character as
      text. <CODE><COLOR> - Set the foreground color. <CODE><COLOR>, - Set the
      foreground color and display the , character as text. <CODE><COLOR>,<COLOR> -
      Set the foreground and background color.
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
            Keep the Forms of Color Codes section above in mind, as this method of
            formatting keeps these same rules – the exceptions being that <CODE>
            represents the hex color character (0x04) and <COLOR> represents a
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

void IRC::onPRIVMSG(IRCMessage &message) {
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

        std::map<std::string, std::string> m = messageToClient(message);
        m["type"] = "message";
        m["text/plain"] = stripFormatting(privmsg);
        m["text/irc"] = privmsg;

        if (mClient)
            mClient->onMessage(m);

    } else {
        // Malformed message?
    }
}

void IRC::onTAGMSG(IRCMessage &message) {}

void IRC::onNOTICE(IRCMessage &message) {
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

        std::map<std::string, std::string> m = messageToClient(message);
        m["type"] = "notice";
        m["text/plain"] = stripFormatting(notice);
        m["text/irc"] = notice;
        if (mClient)
            mClient->onMessage(m);

    } else {
        // Malformed message?
    }
}
void IRC::onERROR(IRCMessage &message) {}

void IRC::onJOIN(IRCMessage &message) {
    if (message.parameters.size() > 0) {
        auto channel = toLower(message.parameters[0]);
        if (isEqual(mNick, message.source.nick)) {
            // We have joined a channel
            ircChannels[channel].joined = true;
            // We index on the lowe case string, we store the string with case
            // preserved for displaying purposes
            ircChannels[toLower(message.parameters[0])].name = message.parameters[0];
            // Obtain the channel modes
            send("MODE " + channel);
        } else {
            // Someone else has joined a channel we are in
            // --> Update channel member list
            IRCUser joined;
            joined.nick = message.source.nick;
            joined.user = message.source.user;
            joined.host = message.source.host;

            if (message.parameters.size() > 2) {
                // extended join
                joined.account = message.parameters[1];
                joined.realname = message.parameters[2];
            }
            ircChannels[channel].nicks[toLower(message.source.nick)] = joined;
        }
    }
}

void IRC::onChannelModeIs(IRCMessage &message) {
    // The RPL_CHANNELMODEIS message looks like
    // "<client> <channel> <modestring> <mode arguments>..."
    // We drop the <client> and let the onMODE handler process it

    if (message.parameters.size() > 0) {
        IRCMessage adjusted = message;
        adjusted.parameters.erase(adjusted.parameters.begin());
        onMODE(adjusted);
    }
}

void IRC::onMODE(IRCMessage &message) {
    if (message.parameters.size() > 1) {
        auto target = message.parameters[0];
        if (isChannel(target)) {
            /*
                    Channel Membership:
                    Server MUST NOT list modes in this parameter that are also
               advertised in the PREFIX parameter. However, modes within the PREFIX
               parameter may be treated as type B modes.


                Type A: Modes that add or remove an address to or from a list. These
               modes MUST always have a parameter when sent from the server to a
               client. A client MAY issue this type of mode without an argument to
               obtain the current contents of the list. The numerics used to retrieve
               contents of Type A modes depends on the specific mode. Also see the
               EXTBAN parameter.

                Type B: Modes that change a setting on a channel. These modes MUST
               always have a parameter.

                Type C: Modes that change a setting on a channel. These modes MUST
               have a parameter when being set, and MUST NOT have a parameter when
               being unset.

                Type D: Modes that change a setting on a channel. These modes MUST NOT
               have a parameter.
            */

            int parameter = 0;
            struct modechange_t {
                char modeset;
                char modechar;
                int parameter;
            };
            std::vector<modechange_t> modeChanges;

            auto modestring = message.parameters[1];

            char modeset = 0;
            unsigned pos = 0;
            while (pos < modestring.length()) {
                char modechar = modestring[pos];
                pos++;
                if (modechar == '+' || modechar == '-') {
                    modeset = modechar;
                    continue;
                }

                if (modeset == '+' || modeset == '-') {

                    modechange_t modechange = {.modeset = modeset, .modechar = modechar, .parameter = -1};

                    if (serverInfo.channelMembershipPrefixes.contains(modechar)) {
                        // Channel Membership
                        // Always takes parameter
                        modechange.parameter = parameter++;
                    }
                    if (serverInfo.channelTypeAModes.find(modechar) != std::string::npos) {
                        // Mode Type A
                        // Add address to list
                        // Always takes parameter in message from server
                        modechange.parameter = parameter++;
                    }
                    if (serverInfo.channelTypeBModes.find(modechar) != std::string::npos) {
                        // Mode Type B
                        // Change setting on channel
                        // Always takes parameter
                        modechange.parameter = parameter++;
                    }
                    if (serverInfo.channelTypeCModes.find(modechar) != std::string::npos) {
                        // Mode Type C
                        // Change setting on channel
                        // Takes parameter when being set
                        // Takes no parameter when being unset
                        if (modeset == '+')
                            modechange.parameter = parameter++;
                    }
                    if (serverInfo.channelTypeDModes.find(modechar) != std::string::npos) {
                        // Mode Type D
                        // Change setting on channel
                        // No parameters
                    }

                    modeChanges.push_back(modechange);
                } else {
                    // Incorrectly formatted modestring
                    return;
                }
            }

            // Now we have created a list of modes to be set and unset, with their
            // respective parameter.

            LOG_DEBUG("Setting modes on channel: %s", target.c_str());
            for (auto &modeChange : modeChanges) {
                std::string modeParameter;
                if (modeChange.parameter != -1) {
                    unsigned parameter = modeChange.parameter + 2;
                    if (message.parameters.size() > parameter) {
                        modeParameter = message.parameters[parameter];
                    } else {
                        modeParameter = "ERROR";
                    }
                }
                LOG_DEBUG("%c%c %s", modeChange.modeset, modeChange.modechar, modeParameter.c_str());
                // TODO: Now we should process that what we have gathered
            }

        } else {
            // Mode on user.
        }
    }
}

void IRC::onTopic(IRCMessage &message) {
    // <client> <channel> :<topic>"
    // Do we have to care about the client in the message?
    if (message.parameters.size() >= 3) {
        ircChannels[toLower(message.parameters[1])].topic = message.parameters[2];
        ircChannels[toLower(message.parameters[1])].topicStripped = stripFormatting(message.parameters[2]);
    }
}

void IRC::onTopicWhoTime(IRCMessage &message) {
    if (message.parameters.size() >= 4) {
        ircChannels[toLower(message.parameters[1])].topicNick = message.parameters[2];
        ircChannels[toLower(message.parameters[1])].topicSetAt = atoi(message.parameters[3].c_str());
    }
}

void IRC::onNamReply(IRCMessage &message) {
    // But I suppose, don't bother, send a WHO command instead
    // "<client> <symbol> <channel> :[prefix]<nick>{ [prefix]<nick>}"
    //	if (message.parameters.size() >= 4) {
    //		auto symbol = toLower(message.parameters[1]);
    //
    //		auto names = splitString(message.parameters[3]);
    //		for (auto &name : names) {
    //			// look up and process prefix
    //			// PREFIX=(qaohv)~&@%+
    //
    //
    //			if (serverInfo.features.count("PREFIX")) {
    //			}
    //		}
    //		mIRCChannels[toLower(message.parameters[2])];
    //	}
}

void IRC::onEndOfNames(IRCMessage &message) {
    if (message.parameters.size() >= 3) {
        if (serverInfo.features.count("WHOX")) {
            // unsigned token = rand() % 100;
            std::random_device r;
            std::default_random_engine e1(r());
            std::uniform_int_distribution<unsigned> uniform_dist(1, 99);
            unsigned token = uniform_dist(e1);
            ircChannels[toLower(message.parameters[1])].token = token;
            send("WHO " + toLower(message.parameters[1]) + " %t%c%u%i%h%s%n%f%d%l%a%o%r," + std::to_string(token));
        } else {
            send("WHO " + toLower(message.parameters[1]));
        }
    }
}

void IRC::onWhoSpcReply(IRCMessage &message) {
    // WHOX Response
    // https://ircv3.net/specs/extensions/whox
    // <client> [token] [channel] [user] [ip] [host] [server] [nick] [flags]
    // [hopcount] [idle] [account] [oplevel] [:realname]

    if (message.parameters.size() > 13) {
        IRCUser user = {};

        auto client = message.parameters[0];
        auto token = message.parameters[1];
        auto channel = message.parameters[2];
        user.user = message.parameters[3];
        user.ip = message.parameters[4];
        user.host = message.parameters[5];
        user.server = message.parameters[6];
        user.nick = message.parameters[7];
        user.flags = message.parameters[8];
        user.hopcount = message.parameters[9];
        user.idle = message.parameters[10];
        user.account = message.parameters[11];
        user.oplevel = message.parameters[12];
        user.realname = message.parameters[13];

        if (isChannel(channel))
            ircChannels[toLower(channel)].nicks[toLower(user.nick)] = user;
    }
}
void IRC::onWhoReply(IRCMessage &message) {
    //   "<client> <channel> <username> <host> <server> <nick> <flags> :<hopcount>
    //   <realname>"
    if (message.parameters.size() > 8) {
        IRCUser user = {};
        auto client = message.parameters[0];
        auto channel = message.parameters[1];
        user.user = message.parameters[2];
        user.host = message.parameters[3];
        user.server = message.parameters[4];
        user.nick = message.parameters[5];
        user.flags = message.parameters[6];
        user.hopcount = message.parameters[7];
        user.realname = message.parameters[8];

        if (isChannel(channel))
            ircChannels[toLower(channel)].nicks[toLower(user.nick)] = user;
    }
}

void IRC::onEndOfWho(IRCMessage &message) {
#ifdef ENABLE_LOG_DEBUG
    if (message.parameters.size() > 1) {
        // "<client> <mask> :End of WHO list"
        auto channel = toLower(message.parameters[1]);
        if (isChannel(channel)) {
            LOG_DEBUG("Channel: %s", channel.c_str());
            for (auto &nick : ircChannels[toLower(channel)].nicks) {
                LOG_DEBUG("Nick: %s", nick.second.nick.c_str());
            }
        }
    }
#endif
}
//

void IRC::onNicknameInUse(IRCMessage &message) {
    if (!serverInfo.registrationComplete && Numeric::ERR_NICKNAMEINUSE == message.command) {
        // TODO, limit retries, make configurable
        mNick += "_";
        send("NICK " + mNick);
        return;
    }
}

void IRC::onMessage(IRCMessage &message) {

    if (mMessageParsers[message.command]) {
        mMessageParsers[message.command](message);
    } else {
        // Unknown message type
    }
}

void IRC::splitUserNickHost(IRCSource &source) {
    auto userPos = source.raw.find("!");
    auto hostPos = source.raw.find("@");
    if (userPos != std::string::npos) {
        source.nick = source.raw.substr(0, userPos);
    } else {
        userPos = 0;
    }
    if (hostPos != std::string::npos) {
        source.user = source.raw.substr(userPos + 1, hostPos - userPos - 1);
        hostPos++;
    } else {
        hostPos = 0;
    }
    source.host = source.raw.substr(hostPos);
}

void IRC::parseMessage(std::string line) {
    // LOG_DEBUG("%s", (">>> " + line).c_str());
    LOG_DEBUG(">>> %s", line.c_str());
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
                auto lineWithoutTag = line.substr(trailingParameterPos + 2);
                auto trainlingParameterPosWithoutTag = lineWithoutTag.find(" :");
                if (trainlingParameterPosWithoutTag != std::string::npos)
                    trailingParameterPos += 2 + lineWithoutTag.find(" :");
                else
                    trailingParameterPos = std::string::npos;
            }
        }

        if (trailingParameterPos != std::string::npos) {
            trailingParameter = line.substr(trailingParameterPos + 2);
            line = line.substr(0, trailingParameterPos);
        }
    }

    auto tokens = splitString(line);

    // Tags, Optional, IRCv3
    // https://defs.ircdocs.horse/defs/tags
    if (tokens.size()) {
        if (tokens[0][0] == '@') {
            // message.tags = tokens[0];
            // message.tags.erase(0, 1); // remove the '@'

            message.tags = parseTags(tokens[0].substr(1));

            tokens.erase(tokens.begin());
        }
    }

    // Source, Optional, RFC 1459
    if (tokens.size()) {
        if (tokens[0][0] == ':') {
            message.source.raw = tokens[0];
            message.source.raw.erase(0, 1); // remove the ':'
            tokens.erase(tokens.begin());

            splitUserNickHost(message.source);
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
    if (trailingParameter.length())
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
    // LOG_DEBUG(("<<< " + message).c_str());
    LOG_DEBUG("<<< %s", message.c_str());
    this->mConnection->sendLine(message);
}

bool IRC::isChannel(const std::string target) {
    if (!target.length())
        return false;
    for (unsigned i = 0; i < serverInfo.features["CHANTYPES"].length(); i++) {
        if (target[0] == serverInfo.features["CHANTYPES"][i])
            return true;
    }
    return false;
}

bool IRC::isNick(const std::string target) {
    if (!target.length())
        return false;
    return !isChannel(target);
    /*


     Nickname grammar in rfc2812
     nickname   =  ( letter / special ) *8( letter / digit / special / "-" )

            letter     =  %x41-5A / %x61-7A       ; A-Z / a-z
            special    =  %x5B-60 / %x7B-7D
                       ; "[", "]", "\", "`", "_", "^", "{", "|", "}"
    */

    /*
    https://github.com/DanielOaks/ircv3-specifications/blob/master%2Brfc7700/documentation/rfc8265.md
    has some nice rules

    */
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

std::string IRC::formatTags(std::map<std::string, std::string> tags) {
    std::string result;
    bool first = true;
    for (auto &tag : tags) {
        if (!first)
            result += ";";
        first = false;
        result += encodeTagKey(tag.first);
        if (tag.second.length())
            result += "=" + encodeTagValue(tag.second);
    }
    return result;
}

void IRC::sendACTION(const std::string target, const std::string text, const std::map<std::string, std::string> tags) {
    sendCTCPQuery(target, "ACTION", text); // TODO tags
}

void IRC::sendPRIVMSG(const std::string target, const std::string text, const std::map<std::string, std::string> tags) {
    if (validTarget(target) && validText(text)) {
        std::string msg;
        if (serverInfo.capabilities.acknowledged.contains("message-tags") && tags.size()) {
            // TODO: escape tags
            // TODO: consider tags as key-value pairs?
            // send("@" + tags + " PRIVMSG " + target + " :" + text);
            msg = "@" + formatTags(tags) + " ";
        }
        msg += "PRIVMSG " + target + " :" + text;
        send(msg);
    }
}

void IRC::sendTAGMSG(const std::string target, const std::map<std::string, std::string> tags) {
    if (validTarget(target)) {
        std::string msg;
        if (serverInfo.capabilities.acknowledged.contains("message-tags") && tags.size()) {
            msg = "@" + formatTags(tags) + " TAGMSG " + target;
            send(msg);
        }
    }
}

void IRC::sendNOTICE(const std::string target, const std::string text, const std::map<std::string, std::string> tags) {
    if (validTarget(target) && validText(text)) {
        std::string msg;
        if (serverInfo.capabilities.acknowledged.contains("message-tags") && tags.size()) {
            msg = "@" + formatTags(tags) + " ";
        }
        msg += "NOTICE " + target + " :" + text;
        send(msg);
    }
}
void IRC::sendCTCPQuery(const std::string target, const std::string command, const std::string parameters) {
    // TODO tags
    if (parameters.length())
        sendPRIVMSG(target, "\01" + command + " " + parameters + "\01");
    else
        sendPRIVMSG(target, "\01" + command + "\01");
}
void IRC::sendCTCPResponse(const std::string target, const std::string command, const std::string parameters) {
    // TODO tags
    if (parameters.length())
        sendNOTICE(target, "\01" + command + " " + parameters + "\01");
    else
        sendNOTICE(target, "\01" + command + "\01");
}

std::map<std::string, std::string> IRC::parseTags(const std::string &tagString) {
    std::map<std::string, std::string> result;
    auto tags = splitString(tagString, ";");
    for (auto &tag : tags) {
        auto kv = splitString(tag, "=", 2);
        std::string key, value;

        // If the tag has a vendor prefix, it might be punycode encoded
        // We might want to decode it. However, how likely is it to
        // encounter such a tag. Therefore this is low priority.
        if (kv.size() > 0)
            key = decodeTagKey(kv[0]);

        if (kv.size() > 1)
            value = decodeTagValue(kv[1]);
        result[key] = value;
    }
    return result;
}

// Parses negations from features (ISUPPORT) or capabilities (CAP *)
std::vector<std::string> IRC::parseNegation(const std::vector<std::string> &tokens) {
    std::vector<std::string> result;
    for (auto &token : tokens) {
        if (token.length() && token[0] == '-') {
            result.push_back(token.substr(1));
        }
    }
    return result;
}

// Parses Key-Value pairs from features (ISUPPORT) or capabilities (CAP *)
// while leaving out negated keys
std::map<std::string, std::string> IRC::parseKeyValue(const std::vector<std::string> &tokens) {
    std::map<std::string, std::string> result;

    std::string key, value;
    for (auto &token : tokens) {
        auto issignpos = token.find('=');
        if (issignpos == std::string::npos) {
            key = token;
            value = "";
        } else {
            key = token.substr(0, issignpos);
            value = token.substr(issignpos + 1);
        }
        LOG_DEBUG("key %s value %s", key.c_str(), value.c_str());
        if (key.length() && key[0] != '-')
            result[key] = value;
    }
    return result;
}

// Escapes Tag Values
std::string IRC::encodeTagValue(const std::string &rawString) {
    // TODO: Implement me
    /*
     * https://ircv3.net/specs/extensions/message-tags
    ; (semicolon)  \: (backslash and colon)
    SPACE          \s
    \ 	           \\
    CR 	           \r
    LF 	           \n
     */

    return rawString;
}

// Unescapes Tag Values
std::string IRC::decodeTagValue(const std::string &escapedString) {
    // TODO: Implement me
    /*
     * https://ircv3.net/specs/extensions/message-tags
    ; (semicolon)  \: (backslash and colon)
    SPACE          \s
    \ 	           \\
    CR 	           \r
    LF 	           \n

    If a lone \ exists at the end of an escaped value (with no escape character
    following it), then there SHOULD be no output character. For example, the
    escaped value test\ should unescape to test. If a \ exists with no valid
    escape character (for example, \b), then the invalid backslash SHOULD be
    dropped. For example, \b should unescape to just b.
     */

    return escapedString;
}

std::string IRC::encodeTagKey(const std::string &rawString) {
    // A non-ascii key needs to be punycode encoded
    // TODO: priority: low
    return rawString;
}

std::string IRC::decodeTagKey(const std::string &escapedString) {
    // Vendor Keys Prefixes, if not ascii, are punycode encoded
    // May need to decode.
    // TODO: priority: low
    return escapedString;
}

// Convert IRC messages to generic format used by the Client class
std::map<std::string, std::string> IRC::messageToClient(IRCMessage &message) {
    std::map<std::string, std::string> result;
    auto target = message.parameters[0];

    if (message.source.nick.length()) {
        result["sender"] = toLower(message.source.nick);
    } else {
        result["sender"] = toLower(message.source.raw);
    }

    result["target"] = toLower(target);
    result["target/irc"] = target;
    if (isChannel(target)) {
        result["target/type"] = "channel";
    } else {
        if (isEqual(mNick, target)) {
            result["target/type"] = "pm";
        } else {
            result["target/type"] = "other";
        }
    }

    if (message.source.nick.length())
        result["sender/irc/nick"] = message.source.nick;
    if (message.source.user.length())
        result["sender/irc/user"] = message.source.user;
    if (message.source.host.length())
        result["sender/irc/host"] = message.source.host;
    if (message.tags.contains("account"))
        result["sender/irc/account"] = message.tags["account"];

    result["sender/irc/raw"] = message.source.raw;

    for (auto &tag : message.tags) {
        result["irc/tag/" + tag.first] = tag.second;
    }
    return result;
}

// Accept generic type message from Client class and send it as an IRC message
// TODO: how to handle incorrect message types? Add a return value, throw an
// exception?
// TODO: tags not supported yet
void IRC::sendMessage(std::map<std::string, std::string> message) {
    if (message.contains("target")) {
        std::string text;

        if (message.contains("text/plain"))
            text = message["text/plain"];

        if (message.contains("text/irc"))
            text = message["text/irc"];

        if (message.contains("type")) {
            if (message["type"] == "message") {
                sendPRIVMSG(message["target"], text);
            }
            if (message["type"] == "notice") {
                sendNOTICE(message["target"], text);
            }
            if (message["type"] == "action") {
                sendACTION(message["target"], text);
            }
        }
    }
}

} // namespace geblaat

#ifdef DYNAMIC_LIBRARY
extern "C" {
geblaat::IRC *newInstance(void) { return new geblaat::IRC(); }
void delInstance(geblaat::IRC *inst) { delete inst; }
pluginloadable_t plugin_info = {
    .name = "IRC",
    .description = "IRC Protocol support",
    .abi = {.abi = pluginloadable_abi_cpp, .version = 0},
};
}
#endif
