#pragma once
#include <pch.hpp>

#include <lsa.hpp>
#include <memory>
#include <string>
#include <vector>

namespace Msv1_0 {
    enum class CacheLogonFlags : ULONG {
        RequestMitLogon = 1,
        RequestInfo4 = 2,
        DeleteEntry = 4,
        RequestSmartcardOnly = 8
    };

    enum class CacheLookupCredType : ULONG {
        None = 0,
        Raw, // Used for public-key smart card data
        Ntowf
    };

    enum class DeriveCredType : ULONG {
        Sha1 = 0,
        Sha1V2
    };

    enum class Lm20ParameterControl : ULONG {
        UsePrimaryPassword = 0x01,
        ReturnPrimaryUsername = 0x02,
        ReturnPrimaryLogonDomainName = 0x04,
        ReturnNonNtUserSessionKey = 0x08,
        GenerateClientChallenge = 0x10,
        GcrNtlm3Parms = 0x20,
        GcrTargetInfo = 0x40, // ServerName field should contains target info AV pairs
        ReturnReservedParameter = 0x80, // Previously 0x10
        GcrAllowNtlm = 0x100, // Allow the use of NTLM
        GcrUseOemSet = 0x200, // Response will use oem character set
        GcrMachineCredential = 0x400,
        GcrUseOwfPassword = 0x800, // Use owf passwords
        GcrAllowLm = 0x1000, // Allow the use of LM 
        // Windows Server 2003 and above
        GcrAllowNoTarget = 0x2000, // allow no target server or target domain name
        // Windows 10 and above
        GcrVsmProtectedPassword = 0x4000 // password is VSM protected
    };

    enum class ProcessOption : ULONG {
        AllowBlankPassword = 0x01,
        DisableAdminLockout = 0x02,
        DisableForceGuest = 0x04,
        AllowOldPassword = 0x08,
        TryCacheFirst = 0x10
    };
    
    typedef enum _MSV1_0_CREDENTIAL_KEY_TYPE {
        InvalidCredKey,
        IUMCredKey,
        DomainUserCredKey,
        LocalUserCredKey,
        ExternallySuppliedCredKey
    } MSV1_0_CREDENTIAL_KEY_TYPE;

    // Redefines MSV1_0_PROTOCOL_MESSAGE_TYPE to ensure all members are included
    enum class PROTOCOL_MESSAGE_TYPE : ULONG {
        Lm20ChallengeRequest = 0,
        Lm20GetChallengeResponse,
        EnumerateUsers,
        GetUserInfo,
        ReLogonUsers,
        ChangePassword,
        ChangeCachedPassword,
        GenericPassthrough,
        CacheLogon,
        SubAuth,
        DeriveCredential,
        CacheLookup,
        SetProcessOption,
        ConfigLocalAliases,
        ClearCachedCredentials,
        LookupToken,
        ValidateAuth,
        CacheLookupEx,
        GetCredentialKey,
        SetThreadOption,
        DecryptDpapiMasterKey,
        GetStrongCredentialKey,
        TransferCred,
        ProvisionTbal,
        DeleteTbalSecrets
    };

    typedef struct _CACHE_LOGON_REQUEST {
        PROTOCOL_MESSAGE_TYPE MessageType{ PROTOCOL_MESSAGE_TYPE::CacheLogon };
        PVOID LogonInformation;
        PVOID ValidationInformation;
        PVOID SupplementalCacheData;
        ULONG SupplementalCacheDataLength;
        ULONG RequestFlags{ 0 };
    } CACHE_LOGON_REQUEST, *PCACHE_LOGON_REQUEST;

    typedef struct _CACHE_LOOKUP_REQUEST {
        PROTOCOL_MESSAGE_TYPE MessageType{ PROTOCOL_MESSAGE_TYPE::CacheLookup };
        UNICODE_STRING UserName;
        UNICODE_STRING DomainName;
        CacheLookupCredType CredentialType;
        ULONG CredentialInfoLength;
        UCHAR CredentialSubmitBuffer[1]; // in-place array of length CredentialInfoLength
    } CACHE_LOOKUP_REQUEST, *PCACHE_LOOKUP_REQUEST;

    typedef struct _CACHE_LOOKUP_RESPONSE {
        PROTOCOL_MESSAGE_TYPE MessageType;
        PVOID ValidationInformation;
        PVOID SupplementalCacheData;
        ULONG SupplementalCacheDataLength;
    } CACHE_LOOKUP_RESPONSE, *PCACHE_LOOKUP_RESPONSE;

    typedef struct _CACHE_LOOKUP_EX_REQUEST : public CACHE_LOOKUP_REQUEST {
        _CACHE_LOOKUP_EX_REQUEST() {
            this->MessageType = PROTOCOL_MESSAGE_TYPE::CacheLookupEx;
        }
    } CACHE_LOOKUP_EX_REQUEST, *PCACHE_LOOKUP_EX_REQUEST;

    typedef struct _CACHE_LOOKUP_EX_RESPONSE : CACHE_LOOKUP_RESPONSE {
    } CACHE_LOOKUP_EX_RESPONSE, *PCACHE_LOOKUP_EX_RESPONSE;

    typedef struct _CLEAR_CACHED_CREDENTIALS_REQUEST {
        PROTOCOL_MESSAGE_TYPE MessageType{ PROTOCOL_MESSAGE_TYPE::ClearCachedCredentials };
    } CLEAR_CACHED_CREDENTIALS_REQUEST, *PCLEAR_CACHED_CREDENTIALS_REQUEST;

    typedef struct _DECRYPT_DPAPI_MASTER_KEY_REQUEST {
        PROTOCOL_MESSAGE_TYPE MessageType{ PROTOCOL_MESSAGE_TYPE::DecryptDpapiMasterKey };
        // OWF password type. 0 for NtOwf, 1 for ShaOwf
        // Based off  NtlmCredIsoInProc::DecryptDpapiMasterKey
        DWORD IsLocalUserCredKey; // 1 means it is a SHA OWF
        LUID LogonSession;
        ULONG bcryptInputDataLength;
        PUCHAR bcryptInputData;
        DWORD cbMasterKeyIn;
        PUCHAR pbMasterKeyIn;
        UCHAR Reserved[32];
    } DECRYPT_DPAPI_MASTER_KEY_REQUEST, *PDECRYPT_DPAPI_MASTER_KEY_REQUEST;

    typedef struct _DECRYPT_DPAPI_MASTER_KEY_RESPONSE {
        PROTOCOL_MESSAGE_TYPE MessageType;
        DWORD KeySize;
        UCHAR Key[];
    } DECRYPT_DPAPI_MASTER_KEY_RESPONSE, *PDECRYPT_DPAPI_MASTER_KEY_RESPONSE;

    typedef struct _DELETE_TBAL_SECRETS_REQUEST {
        PROTOCOL_MESSAGE_TYPE MessageType{ PROTOCOL_MESSAGE_TYPE::DeleteTbalSecrets };
    } DELETE_TBAL_SECRETS_REQUEST, *PDELETE_TBAL_SECRETS_REQUEST;

    typedef struct _DERIVECRED_REQUEST {
        PROTOCOL_MESSAGE_TYPE MessageType{ PROTOCOL_MESSAGE_TYPE::DeriveCredential };
        LUID LogonSession;
        ULONG DeriveCredType;
        ULONG DeriveCredInfoLength;
        UCHAR DeriveCredSubmitBuffer[1];
    } DERIVECRED_REQUEST, *PDERIVECRED_REQUEST;

    typedef struct _DERIVECRED_RESPONSE {
        MSV1_0_PROTOCOL_MESSAGE_TYPE MessageType;
        ULONG DeriveCredInfoLength;
        UCHAR DeriveCredReturnBuffer[1];
    } DERIVECRED_RESPONSE, *PDERIVECRED_RESPONSE;

    typedef struct _ENUMUSERS_REQUEST {
        PROTOCOL_MESSAGE_TYPE MessageType{ PROTOCOL_MESSAGE_TYPE::EnumerateUsers };
    } ENUMUSERS_REQUEST, *PENUMUSERS_REQUEST;

    typedef struct _ENUMUSERS_RESPONSE {
        PROTOCOL_MESSAGE_TYPE MessageType;
        ULONG NumberOfLoggedOnUsers;
        PLUID LogonSessions;
        PULONG EnumHandles;
    } ENUMUSERS_RESPONSE, *PENUMUSERS_RESPONSE;

    typedef struct _GET_CREDENTIAL_KEY_REQUEST {
        PROTOCOL_MESSAGE_TYPE MessageType{ PROTOCOL_MESSAGE_TYPE::GetCredentialKey };
        LUID LogonSession;
        UCHAR Reserved[16] = { 0 };
    } GET_CREDENTIAL_KEY_REQUEST, *PGET_CREDENTIAL_KEY_REQUEST;

    typedef struct _GET_CREDENTIAL_KEY_RESPONSE {
        PROTOCOL_MESSAGE_TYPE MessageType;
        UCHAR Reserved[16];
        DWORD CredSize; // <- 0x28
        UCHAR ShaPassword[MSV1_0_SHA_PASSWORD_LENGTH];
        UCHAR Key2[20];
        // 8 bytes of pad
    } GET_CREDENTIAL_KEY_RESPONSE, *PGET_CREDENTIAL_KEY_RESPONSE;

    typedef struct _GETUSERINFO_REQUEST {
        PROTOCOL_MESSAGE_TYPE MessageType{ PROTOCOL_MESSAGE_TYPE::GetUserInfo };
        LUID LogonSession;
    } GETUSERINFO_REQUEST, *PGETUSERINFO_REQUEST;

    typedef struct _GETUSERINFO_RESPONSE {
        PROTOCOL_MESSAGE_TYPE MessageType;
        PSID UserSid;
        UNICODE_STRING UserName;
        UNICODE_STRING LogonDomainName;
        UNICODE_STRING LogonServer;
        SECURITY_LOGON_TYPE LogonType;
    } GETUSERINFO_RESPONSE, *PGETUSERINFO_RESPONSE;

    typedef struct _GET_STRONG_CREDENTIAL_KEY_REQUEST {
        PROTOCOL_MESSAGE_TYPE MessageType{ PROTOCOL_MESSAGE_TYPE::GetStrongCredentialKey };
        DWORD Version; // Specifies the mode of operation
        // Used in version 0 requests
        DWORD Reserved[8]; // Ignored
        LUID LogonId;
        // Used in version 1 requests
        MSV1_0_CREDENTIAL_KEY_TYPE KeyType; // Must be DomainUserCredKey or LocalUserCredKey
        DWORD KeyLength;
        PWSTR Key; // Treated as a cleartext password for DomainUserCredKey, otherwise an NT OWF hash
        DWORD SidLength;
        PWSTR Sid; // Used to lookup the account type to 
        DWORD IsProtectedUser; // Determined from lsasrv!LsapGetStrongCredentialKeyFromMSV
    } GET_STRONG_CREDENTIAL_KEY_REQUEST, *PGET_STRONG_CREDENTIAL_KEY_REQUEST;

    using GET_STRONG_CREDENTIAL_KEY_RESPONSE = GET_CREDENTIAL_KEY_RESPONSE;
    using PGET_STRONG_CREDENTIAL_KEY_RESPONSE = PGET_CREDENTIAL_KEY_RESPONSE;

    typedef struct _LM20_CHALLENGE_REQUEST_REQUEST {
        PROTOCOL_MESSAGE_TYPE MessageType{ PROTOCOL_MESSAGE_TYPE::Lm20ChallengeRequest };
    } LM20_CHALLENGE_REQUEST_REQUEST, *PLM20_CHALLENGE_REQUEST_REQUEST;

    typedef struct _LM20_CHALLENGE_REQUEST_RESPONSE {
        PROTOCOL_MESSAGE_TYPE MessageType;
        UCHAR ChallengeToClient[8]; // challenge length
    } LM20_CHALLENGE_REQUEST_RESPONSE, *PLM20_CHALLENGE_REQUEST_RESPONSE;

    typedef struct _LM20_GET_CHALLENGE_RESPONSE_REQUEST {
        PROTOCOL_MESSAGE_TYPE MessageType{ PROTOCOL_MESSAGE_TYPE::Lm20GetChallengeResponse };
        ULONG ParameterControl;
        LUID LogonId;
        UNICODE_STRING Password;
        UCHAR ChallengeToClient[MSV1_0_CHALLENGE_LENGTH];
        // Rest is only present if ParameterControl includes GCR_NTLM3_PARMS (0x20)
        UNICODE_STRING UserName;
        UNICODE_STRING LogonDomainName;
        UNICODE_STRING ServerName; // server domain or target info AV pairs
    } LM20_GET_CHALLENGE_RESPONSE_REQUEST, *PLM20_GET_CHALLENGE_RESPONSE_REQUEST;

    typedef struct _LM20_GET_CHALLENGE_RESPONSE_RESPONSE {
        MSV1_0_PROTOCOL_MESSAGE_TYPE MessageType;
        STRING CaseSensitiveChallengeResponse;
        STRING CaseInsensitiveChallengeResponse;
        UNICODE_STRING UserName;
        UNICODE_STRING LogonDomainName;
        UCHAR UserSessionKey[MSV1_0_USER_SESSION_KEY_LENGTH];
        UCHAR LanmanSessionKey[MSV1_0_LANMAN_SESSION_KEY_LENGTH];
    } LM20_GET_CHALLENGE_RESPONSE_RESPONSE, *PLM20_GET_CHALLENGE_RESPONSE_RESPONSE;

    typedef struct _PASSTHROUGH_REQUEST {
        PROTOCOL_MESSAGE_TYPE MessageType{ PROTOCOL_MESSAGE_TYPE::GenericPassthrough };
        UNICODE_STRING DomainName;
        UNICODE_STRING PackageName;
        ULONG DataLength;
        PUCHAR LogonData;
        ULONG Pad{ 0 };
    } PASSTHROUGH_REQUEST, *PPASSTHROUGH_REQUEST;

    typedef struct _PASSTHROUGH_RESPONSE {
        PROTOCOL_MESSAGE_TYPE MessageType;
        ULONG Pad{ 0 };
        ULONG DataLength;
        PUCHAR ValidationData; // The response data
    } PASSTHROUGH_RESPONSE, *PPASSTHROUGH_RESPONSE;

    typedef struct _PROVISION_TBAL_REQUEST {
        PROTOCOL_MESSAGE_TYPE MessageType{ PROTOCOL_MESSAGE_TYPE::ProvisionTbal };
        LUID LogonSession;
    } PROVISION_TBAL_REQUEST, *PPROVISION_TBAL_REQUEST;

    typedef struct _SETPROCESSOPTION_REQUEST {
        PROTOCOL_MESSAGE_TYPE MessageType{ PROTOCOL_MESSAGE_TYPE::SetProcessOption };
        ULONG ProcessOptions{ 0 };
        BOOLEAN DisableOptions;
    } SETPROCESSOPTION_REQUEST, *PSETPROCESSOPTION_REQUEST;

    typedef struct _SETTHREADOPTION_REQUEST {
        PROTOCOL_MESSAGE_TYPE MessageType{ PROTOCOL_MESSAGE_TYPE::SetProcessOption };
        ULONG ThreadOptions{ 0 };
        BOOLEAN DisableOptions; // correct
        BOOLEAN unknown; // something else
    } SETTHREADOPTION_REQUEST, *PSETTHREADOPTION_REQUEST;

    class Proxy {
    public:
        Proxy(const std::shared_ptr<Lsa>& lsa);

        // A subset of the supported functions in msv1_0
        bool CacheLogon(void* logonInfo, void* validationInfo, const std::vector<byte>& supplementalCacheData, ULONG flags) const;
        bool CacheLookupEx(const std::wstring username, const std::wstring domain, CacheLookupCredType type, const std::string credential) const;
        bool ChangePassword() const;
        bool ChangeCachedPassword(const std::wstring& domainName, const std::wstring& accountName, const std::wstring& newPassword) const;
        bool ClearCachedCredentials() const;
        bool DecryptDpapiMasterKey() const;
        bool DeleteTbalSecrets() const;
        bool DeriveCredential(PLUID luid, DeriveCredType type, const std::vector<byte>& mixingBits) const;
        bool EnumerateUsers() const;
        bool GenericPassthrough(const std::wstring& domainName, const std::wstring& packageName, std::vector<byte>& data) const;
        bool GetCredentialKey(PLUID luid) const;
        bool GetStrongCredentialKey(PLUID luid, bool isProtectedUser) const;
        bool GetUserInfo(PLUID luid) const;
        bool Lm20ChallengeRequest() const;
        bool Lm20GetChallengeResponse(ULONG flags, PLUID luid, const std::vector<byte>& challenge) const;
        bool ProvisionTbal(PLUID luid) const;
        bool SetProcessOption(ProcessOption options, bool disable) const;

    protected:
        std::shared_ptr<Lsa> lsa;

    private:
        // You must free all returnBuffer outputs with LsaFreeReturnBuffer
        bool CallPackage(const std::string& submitBuffer, void** returnBuffer) const;
        template<typename _Request, typename _Response>
        bool CallPackage(const _Request& submitBuffer, _Response** returnBuffer) const;
        template<typename _Request, typename _Response>
        bool CallPackage(_Request* submitBuffer, size_t submitBufferLength, _Response** returnBuffer) const;
    };
}