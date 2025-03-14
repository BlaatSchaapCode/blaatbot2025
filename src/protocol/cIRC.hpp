/*
 * cIRC.hpp
 *
 *  Created on: 8 mrt. 2025
 *      Author: andre
 */

#ifndef PROTOCOL_CIRC_HPP_
#define PROTOCOL_CIRC_HPP_

#include "cProtocol.hpp"
#include "cconnection.hpp"

#include <functional>
#include <map>
#include <memory>
#include <string>
#include <vector>

namespace protocol {

class cIRC : public cProtocol {
  public:
    cIRC(std::string nick, std::string user = "");
    virtual ~cIRC();

    struct IRCMessage {
        std::string tags;

        std::string source;
        // source is split up into nick, user and host
        std::string nick;
        std::string user;
        std::string host;

        std::string command;
        std::vector<std::string> parameters;
    };

    struct CTCPMessage {
    	std::string command;
    	std::string parameters;
    };

    struct Numeric {
        static constexpr std::string RPL_WELCOME = "001";
        static constexpr std::string RPL_YOURHOST = "002";
        static constexpr std::string RPL_CREATED = "003";
        static constexpr std::string RPL_MYINFO = "004";
        static constexpr std::string RPL_ISUPPORT = "005";
        static constexpr std::string RPL_BOUNCE = "010";
        static constexpr std::string RPL_STATSCOMMANDS = "212";
        static constexpr std::string RPL_ENDOFSTATS = "219";
        static constexpr std::string RPL_UMODEIS = "221";
        static constexpr std::string RPL_STATSUPTIME = "242";
        static constexpr std::string RPL_LUSERCLIENT = "251";
        static constexpr std::string RPL_LUSEROP = "252";
        static constexpr std::string RPL_LUSERUNKNOWN = "253";
        static constexpr std::string RPL_LUSERCHANNELS = "254";
        static constexpr std::string RPL_LUSERME = "255";
        static constexpr std::string RPL_ADMINME = "256";
        static constexpr std::string RPL_ADMINLOC1 = "257";
        static constexpr std::string RPL_ADMINLOC2 = "258";
        static constexpr std::string RPL_ADMINEMAIL = "259";
        static constexpr std::string RPL_TRYAGAIN = "263";
        static constexpr std::string RPL_LOCALUSERS = "265";
        static constexpr std::string RPL_GLOBALUSERS = "266";
        static constexpr std::string RPL_WHOISCERTFP = "276";
        static constexpr std::string RPL_NONE = "300";
        static constexpr std::string RPL_AWAY = "301";
        static constexpr std::string RPL_USERHOST = "302";
        static constexpr std::string RPL_UNAWAY = "305";
        static constexpr std::string RPL_NOWAWAY = "306";
        static constexpr std::string RPL_WHOISREGNICK = "307";
        static constexpr std::string RPL_WHOISUSER = "311";
        static constexpr std::string RPL_WHOISSERVER = "312";
        static constexpr std::string RPL_WHOISOPERATOR = "313";
        static constexpr std::string RPL_WHOWASUSER = "314";
        static constexpr std::string RPL_ENDOFWHO = "315";
        static constexpr std::string RPL_WHOISIDLE = "317";
        static constexpr std::string RPL_ENDOFWHOIS = "318";
        static constexpr std::string RPL_WHOISCHANNELS = "319";
        static constexpr std::string RPL_WHOISSPECIAL = "320";
        static constexpr std::string RPL_LISTSTART = "321";
        static constexpr std::string RPL_LIST = "322";
        static constexpr std::string RPL_LISTEND = "323";
        static constexpr std::string RPL_CHANNELMODEIS = "324";
        static constexpr std::string RPL_CREATIONTIME = "329";
        static constexpr std::string RPL_WHOISACCOUNT = "330";
        static constexpr std::string RPL_NOTOPIC = "331";
        static constexpr std::string RPL_TOPIC = "332";
        static constexpr std::string RPL_TOPICWHOTIME = "333";
        static constexpr std::string RPL_INVITELIST = "336";
        static constexpr std::string RPL_ENDOFINVITELIST = "337";
        static constexpr std::string RPL_WHOISACTUALLY = "338";
        static constexpr std::string RPL_INVITING = "341";
        static constexpr std::string RPL_INVEXLIST = "346";
        static constexpr std::string RPL_ENDOFINVEXLIST = "347";
        static constexpr std::string RPL_EXCEPTLIST = "348";
        static constexpr std::string RPL_ENDOFEXCEPTLIST = "349";
        static constexpr std::string RPL_VERSION = "351";
        static constexpr std::string RPL_WHOREPLY = "352";
        static constexpr std::string RPL_NAMREPLY = "353";
        static constexpr std::string RPL_LINKS = "364";
        static constexpr std::string RPL_ENDOFLINKS = "365";
        static constexpr std::string RPL_ENDOFNAMES = "366";
        static constexpr std::string RPL_BANLIST = "367";
        static constexpr std::string RPL_ENDOFBANLIST = "368";
        static constexpr std::string RPL_ENDOFWHOWAS = "369";
        static constexpr std::string RPL_INFO = "371";
        static constexpr std::string RPL_MOTD = "372";
        static constexpr std::string RPL_ENDOFINFO = "374";
        static constexpr std::string RPL_MOTDSTART = "375";
        static constexpr std::string RPL_ENDOFMOTD = "376";
        static constexpr std::string RPL_WHOISHOST = "378";
        static constexpr std::string RPL_WHOISMODES = "379";
        static constexpr std::string RPL_YOUREOPER = "381";
        static constexpr std::string RPL_REHASHING = "382";
        static constexpr std::string RPL_TIME = "391";
        static constexpr std::string ERR_UNKNOWNERROR = "400";
        static constexpr std::string ERR_NOSUCHNICK = "401";
        static constexpr std::string ERR_NOSUCHSERVER = "402";
        static constexpr std::string ERR_NOSUCHCHANNEL = "403";
        static constexpr std::string ERR_CANNOTSENDTOCHAN = "404";
        static constexpr std::string ERR_TOOMANYCHANNELS = "405";
        static constexpr std::string ERR_WASNOSUCHNICK = "406";
        static constexpr std::string ERR_NOORIGIN = "409";
        static constexpr std::string ERR_NORECIPIENT = "411";
        static constexpr std::string ERR_NOTEXTTOSEND = "412";
        static constexpr std::string ERR_INPUTTOOLONG = "417";
        static constexpr std::string ERR_UNKNOWNCOMMAND = "421";
        static constexpr std::string ERR_NOMOTD = "422";
        static constexpr std::string ERR_NONICKNAMEGIVEN = "431";
        static constexpr std::string ERR_ERRONEUSNICKNAME = "432";
        static constexpr std::string ERR_NICKNAMEINUSE = "433";
        static constexpr std::string ERR_NICKCOLLISION = "436";
        static constexpr std::string ERR_USERNOTINCHANNEL = "441";
        static constexpr std::string ERR_NOTONCHANNEL = "442";
        static constexpr std::string ERR_USERONCHANNEL = "443";
        static constexpr std::string ERR_NOTREGISTERED = "451";
        static constexpr std::string ERR_NEEDMOREPARAMS = "461";
        static constexpr std::string ERR_ALREADYREGISTERED = "462";
        static constexpr std::string ERR_PASSWDMISMATCH = "464";
        static constexpr std::string ERR_YOUREBANNEDCREEP = "465";
        static constexpr std::string ERR_CHANNELISFULL = "471";
        static constexpr std::string ERR_UNKNOWNMODE = "472";
        static constexpr std::string ERR_INVITEONLYCHAN = "473";
        static constexpr std::string ERR_BANNEDFROMCHAN = "474";
        static constexpr std::string ERR_BADCHANNELKEY = "475";
        static constexpr std::string ERR_BADCHANMASK = "476";
        static constexpr std::string ERR_NOPRIVILEGES = "481";
        static constexpr std::string ERR_CHANOPRIVSNEEDED = "482";
        static constexpr std::string ERR_CANTKILLSERVER = "483";
        static constexpr std::string ERR_NOOPERHOST = "491";
        static constexpr std::string ERR_UMODEUNKNOWNFLAG = "501";
        static constexpr std::string ERR_USERSDONTMATCH = "502";
        static constexpr std::string ERR_HELPNOTFOUND = "524";
        static constexpr std::string ERR_INVALIDKEY = "525";
        static constexpr std::string RPL_STARTTLS = "670";
        static constexpr std::string RPL_WHOISSECURE = "671";
        static constexpr std::string ERR_STARTTLS = "691";
        static constexpr std::string ERR_INVALIDMODEPARAM = "696";
        static constexpr std::string RPL_HELPSTART = "704";
        static constexpr std::string RPL_HELPTXT = "705";
        static constexpr std::string RPL_ENDOFHELP = "706";
        static constexpr std::string ERR_NOPRIVS = "723";
        static constexpr std::string RPL_LOGGEDIN = "900";
        static constexpr std::string RPL_LOGGEDOUT = "901";
        static constexpr std::string ERR_NICKLOCKED = "902";
        static constexpr std::string RPL_SASLSUCCESS = "903";
        static constexpr std::string ERR_SASLFAIL = "904";
        static constexpr std::string ERR_SASLTOOLONG = "905";
        static constexpr std::string ERR_SASLABORTED = "906";
        static constexpr std::string ERR_SASLALREADY = "907";
        static constexpr std::string RPL_SASLMECHS = "908";
    };

    void onData(std::vector<char> data) override;
    void onConnected() override;
    void onDisconnected() override;

  private:
    using IRCMessageParser = std::function<void(const IRCMessage message)>;
    std::map<std::string, IRCMessageParser> mMessageParsers;

    std::vector<char> mBuffer;

    std::string mPass;
    std::string mUser;
    std::string mNick;
    std::string mRealName;

    bool mServerSupportsCapabilities = false;
    std::vector<std::string> mServerCapabilities;

    void parseMessage(std::string message);
    void onMessage(const IRCMessage message);
    void send(std::string message);

    void onMessageDuringRegistration(const IRCMessage message);
    void onPING(const IRCMessage message);
    void onCAP(const IRCMessage message);
    void onERROR(const IRCMessage message);

    void onPRIVMSG(const IRCMessage message);
    void onNOTICE(const IRCMessage message);
    void onCTCPQuery(const IRCMessage message, const CTCPMessage ctcp);
    void onCTCPResponse(const IRCMessage message, const CTCPMessage ctcp);

    void sendPRIVMSG(const std::string target, const std::string text);
    void sendNOTICE(const std::string target, const std::string text);
    void sendCTCPQuery(const std::string target, const std::string text);
    void sendCTCPResponse(const std::string target, const std::string text);

    bool validTarget(const std::string target);
    bool validText(const std::string text);

    std::string stripFormatting(const std::string formattedString);
};

} // namespace protocol
#endif /* PROTOCOL_CIRC_HPP_ */
