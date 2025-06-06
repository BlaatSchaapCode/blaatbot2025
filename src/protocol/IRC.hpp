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

#ifndef PROTOCOL_IRC_HPP_
#define PROTOCOL_IRC_HPP_

#include <functional>
#include <map>
#include <memory>
#include <set>
#include <string>
#include <vector>

#include "C2SProtocol.hpp"
#include "Connection.hpp"
#include "timer.hpp"

namespace geblaat {

class IRC : public C2SProtocol {
  public:
    struct IRCSource {
        std::string raw;
        // source is split up into nick, user and host
        std::string nick;
        std::string user;
        std::string host;
    };

    struct IRCtag {
        std::string tag;
        std::string value;
    };

    struct IRCMessage {
        std::string raw;
        std::map<std::string, std::string> tags;
        IRCSource source;
        std::string command;
        std::vector<std::string> parameters;
    };

    struct CTCPMessage {
        std::string command;
        std::string parameters;
    };

    struct Numeric {
        // Originally these were "static constexpr std::string"
        // However, this is not compatible with the C++ library used
        // on HaikuOS. Therefore these are now
        // "static constexpr const char*" instead.
        static constexpr const char *RPL_WELCOME = "001";
        static constexpr const char *RPL_YOURHOST = "002";
        static constexpr const char *RPL_CREATED = "003";
        static constexpr const char *RPL_MYINFO = "004";
        static constexpr const char *RPL_ISUPPORT = "005";
        static constexpr const char *RPL_BOUNCE = "010";
        static constexpr const char *RPL_STATSCOMMANDS = "212";
        static constexpr const char *RPL_ENDOFSTATS = "219";
        static constexpr const char *RPL_UMODEIS = "221";
        static constexpr const char *RPL_STATSUPTIME = "242";
        static constexpr const char *RPL_LUSERCLIENT = "251";
        static constexpr const char *RPL_LUSEROP = "252";
        static constexpr const char *RPL_LUSERUNKNOWN = "253";
        static constexpr const char *RPL_LUSERCHANNELS = "254";
        static constexpr const char *RPL_LUSERME = "255";
        static constexpr const char *RPL_ADMINME = "256";
        static constexpr const char *RPL_ADMINLOC1 = "257";
        static constexpr const char *RPL_ADMINLOC2 = "258";
        static constexpr const char *RPL_ADMINEMAIL = "259";
        static constexpr const char *RPL_TRYAGAIN = "263";
        static constexpr const char *RPL_LOCALUSERS = "265";
        static constexpr const char *RPL_GLOBALUSERS = "266";
        static constexpr const char *RPL_WHOISCERTFP = "276";
        static constexpr const char *RPL_NONE = "300";
        static constexpr const char *RPL_AWAY = "301";
        static constexpr const char *RPL_USERHOST = "302";
        static constexpr const char *RPL_UNAWAY = "305";
        static constexpr const char *RPL_NOWAWAY = "306";
        static constexpr const char *RPL_WHOISREGNICK = "307";
        static constexpr const char *RPL_WHOISUSER = "311";
        static constexpr const char *RPL_WHOISSERVER = "312";
        static constexpr const char *RPL_WHOISOPERATOR = "313";
        static constexpr const char *RPL_WHOWASUSER = "314";
        static constexpr const char *RPL_ENDOFWHO = "315";
        static constexpr const char *RPL_WHOISIDLE = "317";
        static constexpr const char *RPL_ENDOFWHOIS = "318";
        static constexpr const char *RPL_WHOISCHANNELS = "319";
        static constexpr const char *RPL_WHOISSPECIAL = "320";
        static constexpr const char *RPL_LISTSTART = "321";
        static constexpr const char *RPL_LIST = "322";
        static constexpr const char *RPL_LISTEND = "323";
        static constexpr const char *RPL_CHANNELMODEIS = "324";
        static constexpr const char *RPL_CREATIONTIME = "329";
        static constexpr const char *RPL_WHOISACCOUNT = "330";
        static constexpr const char *RPL_NOTOPIC = "331";
        static constexpr const char *RPL_TOPIC = "332";
        static constexpr const char *RPL_TOPICWHOTIME = "333";
        static constexpr const char *RPL_INVITELIST = "336";
        static constexpr const char *RPL_ENDOFINVITELIST = "337";
        static constexpr const char *RPL_WHOISACTUALLY = "338";
        static constexpr const char *RPL_INVITING = "341";
        static constexpr const char *RPL_INVEXLIST = "346";
        static constexpr const char *RPL_ENDOFINVEXLIST = "347";
        static constexpr const char *RPL_EXCEPTLIST = "348";
        static constexpr const char *RPL_ENDOFEXCEPTLIST = "349";
        static constexpr const char *RPL_VERSION = "351";
        static constexpr const char *RPL_WHOREPLY = "352";
        static constexpr const char *RPL_NAMREPLY = "353";
        static constexpr const char *RPL_WHOSPCRPL = "354";
        static constexpr const char *RPL_LINKS = "364";
        static constexpr const char *RPL_ENDOFLINKS = "365";
        static constexpr const char *RPL_ENDOFNAMES = "366";
        static constexpr const char *RPL_BANLIST = "367";
        static constexpr const char *RPL_ENDOFBANLIST = "368";
        static constexpr const char *RPL_ENDOFWHOWAS = "369";
        static constexpr const char *RPL_INFO = "371";
        static constexpr const char *RPL_MOTD = "372";
        static constexpr const char *RPL_ENDOFINFO = "374";
        static constexpr const char *RPL_MOTDSTART = "375";
        static constexpr const char *RPL_ENDOFMOTD = "376";
        static constexpr const char *RPL_WHOISHOST = "378";
        static constexpr const char *RPL_WHOISMODES = "379";
        static constexpr const char *RPL_YOUREOPER = "381";
        static constexpr const char *RPL_REHASHING = "382";
        static constexpr const char *RPL_TIME = "391";
        static constexpr const char *ERR_UNKNOWNERROR = "400";
        static constexpr const char *ERR_NOSUCHNICK = "401";
        static constexpr const char *ERR_NOSUCHSERVER = "402";
        static constexpr const char *ERR_NOSUCHCHANNEL = "403";
        static constexpr const char *ERR_CANNOTSENDTOCHAN = "404";
        static constexpr const char *ERR_TOOMANYCHANNELS = "405";
        static constexpr const char *ERR_WASNOSUCHNICK = "406";
        static constexpr const char *ERR_NOORIGIN = "409";
        static constexpr const char *ERR_NORECIPIENT = "411";
        static constexpr const char *ERR_NOTEXTTOSEND = "412";
        static constexpr const char *ERR_INPUTTOOLONG = "417";
        static constexpr const char *ERR_UNKNOWNCOMMAND = "421";
        static constexpr const char *ERR_NOMOTD = "422";
        static constexpr const char *ERR_NONICKNAMEGIVEN = "431";
        static constexpr const char *ERR_ERRONEUSNICKNAME = "432";
        static constexpr const char *ERR_NICKNAMEINUSE = "433";
        static constexpr const char *ERR_NICKCOLLISION = "436";
        static constexpr const char *ERR_USERNOTINCHANNEL = "441";
        static constexpr const char *ERR_NOTONCHANNEL = "442";
        static constexpr const char *ERR_USERONCHANNEL = "443";
        static constexpr const char *ERR_NOTREGISTERED = "451";
        static constexpr const char *ERR_NEEDMOREPARAMS = "461";
        static constexpr const char *ERR_ALREADYREGISTERED = "462";
        static constexpr const char *ERR_PASSWDMISMATCH = "464";
        static constexpr const char *ERR_YOUREBANNEDCREEP = "465";
        static constexpr const char *ERR_CHANNELISFULL = "471";
        static constexpr const char *ERR_UNKNOWNMODE = "472";
        static constexpr const char *ERR_INVITEONLYCHAN = "473";
        static constexpr const char *ERR_BANNEDFROMCHAN = "474";
        static constexpr const char *ERR_BADCHANNELKEY = "475";
        static constexpr const char *ERR_BADCHANMASK = "476";
        static constexpr const char *ERR_NOPRIVILEGES = "481";
        static constexpr const char *ERR_CHANOPRIVSNEEDED = "482";
        static constexpr const char *ERR_CANTKILLSERVER = "483";
        static constexpr const char *ERR_NOOPERHOST = "491";
        static constexpr const char *ERR_UMODEUNKNOWNFLAG = "501";
        static constexpr const char *ERR_USERSDONTMATCH = "502";
        static constexpr const char *ERR_HELPNOTFOUND = "524";
        static constexpr const char *ERR_INVALIDKEY = "525";
        static constexpr const char *RPL_STARTTLS = "670";
        static constexpr const char *RPL_WHOISSECURE = "671";
        static constexpr const char *ERR_STARTTLS = "691";
        static constexpr const char *ERR_INVALIDMODEPARAM = "696";
        static constexpr const char *RPL_HELPSTART = "704";
        static constexpr const char *RPL_HELPTXT = "705";
        static constexpr const char *RPL_ENDOFHELP = "706";
        static constexpr const char *ERR_NOPRIVS = "723";
        static constexpr const char *RPL_LOGGEDIN = "900";
        static constexpr const char *RPL_LOGGEDOUT = "901";
        static constexpr const char *ERR_NICKLOCKED = "902";
        static constexpr const char *RPL_SASLSUCCESS = "903";
        static constexpr const char *ERR_SASLFAIL = "904";
        static constexpr const char *ERR_SASLTOOLONG = "905";
        static constexpr const char *ERR_SASLABORTED = "906";
        static constexpr const char *ERR_SASLALREADY = "907";
        static constexpr const char *RPL_SASLMECHS = "908";

        // Microsoft IRCX
        // 8xx Range is IRCX messages
        static constexpr const char *IRCRPL_IRCX = "800";
        static constexpr const char *IRCRPL_ACCESSADD = "801";
        static constexpr const char *IRCRPL_ACCESSDELETE = "802";
        static constexpr const char *IRCRPL_ACCESSSTART = "803";
        static constexpr const char *IRCRPL_ACCESSLIST = "804";
        static constexpr const char *IRCRPL_ACCESSEND = "805";
        static constexpr const char *IRCRPL_EVENTADD = "806";
        static constexpr const char *IRCRPL_EVENTDEL = "807";
        static constexpr const char *IRCRPL_EVENTSTART = "808";
        static constexpr const char *IRCRPL_EVENTLIST = "809";
        static constexpr const char *IRCRPL_EVENTEND = "810";
        static constexpr const char *IRCRPL_LISTXSTART = "811";
        static constexpr const char *IRCRPL_LISTXLIST = "812";
        static constexpr const char *IRCRPL_LISTXPICS = "813";
        static constexpr const char *IRCRPL_LISTXTRUNC = "816";
        static constexpr const char *IRCRPL_LISTXEND = "817";
        static constexpr const char *IRCRPL_PROPLIST = "818";
        static constexpr const char *IRCRPL_PROPEND = "819";

        // The IRCX Error Message range is 9xx. Conflicts with SASL messages
        static constexpr const char *IRCERR_BADCOMMAND = "900";
        static constexpr const char *IRCERR_TOOMANYARGUMENTS = "901";
        static constexpr const char *IRCERR_BADFUNCTION = "902";
        static constexpr const char *IRCERR_BADLEVEL = "903";
        static constexpr const char *IRCERR_BADTAG = "904";
        static constexpr const char *IRCERR_BADPROPERTY = "905";
        static constexpr const char *IRCERR_BADVALUE = "906";
        static constexpr const char *IRCERR_RESOURCE = "907";
        static constexpr const char *IRCERR_SECURITY = "908";
        static constexpr const char *IRCERR_ALREADYAUTHENTICATED = "909";
        static constexpr const char *IRCERR_AUTHENTICATIONFAILED = "910";
        static constexpr const char *IRCERR_AUTHENTICATIONSUSPENDED = "911";
        static constexpr const char *IRCERR_UNKNOWNPACKAGE = "912";
        static constexpr const char *IRCERR_NOACCESS = "913";
        static constexpr const char *IRCERR_DUPACCESS = "914";
        static constexpr const char *IRCERR_MISACCESS = "915";
        static constexpr const char *IRCERR_TOOMANYACCESSES = "916";
        static constexpr const char *IRCERR_EVENTDUP = "918";
        static constexpr const char *IRCERR_EVENTMIS = "919";
        static constexpr const char *IRCERR_NOSUCHEVENT = "920";
        static constexpr const char *IRCERR_TOOMANYEVENTS = "921";
        static constexpr const char *IRCERR_NOWHISPER = "923";
        static constexpr const char *IRCERR_NOSUCHOBJECT = "924";
        static constexpr const char *IRCERR_NOTSUPPORTED = "925";
        static constexpr const char *IRCERR_CHANNELEXIST = "926";
        static constexpr const char *IRCERR_ALREADYONCHANNEL = "927";
        static constexpr const char *IRCERR_UNKNOWNERROR = "999";
    };

    struct IRCUser {
        std::string user;
        std::string ip;
        std::string host;
        std::string server;
        std::string nick;
        std::string flags;
        std::string hopcount;
        std::string idle;
        std::string account;
        std::string oplevel;
        std::string realname;
    };

    struct IRCChannel {
        bool joined;
        std::string name; // preserves case
        std::string topic;
        std::string topicStripped;
        std::string topicNick;
        time_t topicSetAt;
        unsigned token;
        std::map<std::string, IRCUser> nicks;
    };
    std::map<std::string, IRCChannel> ircChannels;

    IRC();
    ~IRC();

    void onData(std::vector<char> data) override;
    void onConnected() override;
    void onDisconnected() override;

    int setConfig(const nlohmann::json &) override;
    nlohmann::json getConfig(void) override { return config; }

  private:
    nlohmann::json config;

    using IRCMessageParser = std::function<void(IRCMessage &message)>;
    std::map<std::string, IRCMessageParser> mMessageParsers;

    Timer connectTimer;
    Timer lagTimer;

    std::vector<char> mBuffer;

    //------------------------------------------------------------------------
    // Todo refactor this into a struct or something
    //------------------------------------------------------------------------
    // NickServ
    std::string mNsPass;

    // Sasl PLain
    std::string mSaslPlainAuthzid;
    std::string mSaslPlainAuthcid;
    std::string mSaslPlainPasswd;

    // Standard IRC
    std::string mPass;
    std::string mUser;
    std::string mNick;
    std::string mRealName;
    //------------------------------------------------------------------------
    struct AutoJoinChannel {
        std::string channel;
        std::string key;
    };
    std::vector<AutoJoinChannel> autoJoinChannels;

    struct {
        bool connected = false;

        bool hasCapabilities = false;
        bool hasExtensions = false;
        bool registrationComplete = false;
        int maxLen = 512;

        std::string network;
        std::string host;
        std::string daemon;
        std::string daemonFamily;

        // From RPL_MYINFO. Might not use there, but info from MYINFO instead
        std::string userModes, channelModes, channelModesWithParameters;

        //
        std::string channelTypeAModes;
        std::string channelTypeBModes;
        std::string channelTypeCModes;
        std::string channelTypeDModes;
        std::map<char, char> channelMembershipPrefixes;

        std::string services;
        std::string servicesFamily;

        std::map<std::string, std::string> features;

        struct {
            std::map<std::string, std::string> supported;
            std::set<std::string> acknowledged;
        } capabilities;

        struct {
            int version;
            bool enabled;
            std::vector<std::string> packages;
            std::vector<std::string> options;
        } extensions;

        std::chrono::milliseconds lag;
    } serverInfo;

    void parseMessage(std::string message);
    void onMessage(IRCMessage &message);
    void send(std::string message);

    void onISupport(IRCMessage &message);

    void onWelcome(IRCMessage &message);
    void onYourHost(IRCMessage &message);
    void onCreated(IRCMessage &message);
    void onMyInfo(IRCMessage &message);

    void onPING(IRCMessage &message);
    void onPONG(IRCMessage &message);
    void onCAP(IRCMessage &message);
    void onIRCX(IRCMessage &message);
    void onERROR(IRCMessage &message);
    void onUnknownCommand(IRCMessage &message);

    void applyFeatures(void);
    void applyServerQuirks(void);
    void onReady(void);
    void onCanRegister(void);

    void onPRIVMSG(IRCMessage &message);
    void onTAGMSG(IRCMessage &message);
    void onNOTICE(IRCMessage &message);
    void onJOIN(IRCMessage &message);
    void onMODE(IRCMessage &message);

    void onTopic(IRCMessage &message);
    void onTopicWhoTime(IRCMessage &message);
    void onChannelModeIs(IRCMessage &message);
    void onNamReply(IRCMessage &message);
    void onEndOfNames(IRCMessage &message);
    void onWhoReply(IRCMessage &message);
    void onWhoSpcReply(IRCMessage &message);
    void onEndOfWho(IRCMessage &message);

    void onCTCPQuery(IRCMessage &message, CTCPMessage &ctcp);
    void onCTCPResponse(IRCMessage &message, CTCPMessage &ctcp);

    void onNicknameInUse(IRCMessage &message);

    void sendPRIVMSG(const std::string target, const std::string text, const std::map<std::string, std::string> tags = {});
    void sendACTION(const std::string target, const std::string text, const std::map<std::string, std::string> tags = {});
    void sendTAGMSG(const std::string target, const std::map<std::string, std::string> tags = {});
    void sendNOTICE(const std::string target, const std::string text, const std::map<std::string, std::string> tags = {});
    void sendCTCPQuery(const std::string target, const std::string command, const std::string parameters = "");
    void sendCTCPResponse(const std::string target, const std::string command, const std::string parameters = "");

    bool isNick(const std::string target);
    bool isChannel(const std::string target);

    bool validTarget(const std::string target);
    bool validText(const std::string text);

    bool isEqual(const std::string first, const std::string seccond);
    std::string toLower(std::string str);

    std::string stripFormatting(const std::string &formattedString);
    void splitUserNickHost(IRCSource &source);
    std::map<std::string, std::string> parseTags(const std::string &tagString);
    std::string formatTags(std::map<std::string, std::string> tags);
    std::map<std::string, std::string> parseKeyValue(const std::vector<std::string> &);
    std::vector<std::string> parseNegation(const std::vector<std::string> &);
    std::string encodeTagValue(const std::string &);
    std::string decodeTagValue(const std::string &);
    std::string encodeTagKey(const std::string &);
    std::string decodeTagKey(const std::string &);

    void requestCapabilityIfPresent(const std::string &);

    void ping();

    std::map<std::string, std::string> messageToClient(IRCMessage &message);

  public:
    void sendMessage(std::map<std::string, std::string> message) override;
};

} // namespace geblaat
#endif /* PROTOCOL_IRC_HPP_ */
