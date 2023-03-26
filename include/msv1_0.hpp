#pragma once
#define _NTDEF_ // Required to include both Ntsecapi and Winternl
#include <Winternl.h>

#include <Ntsecapi.h>
#include <cxxopts.hpp>
#include <lsa.hpp>
#include <memory>
#include <netlogon.hpp>
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

    enum class ProcessOption : ULONG {
        AllowBlankPassword = 0x01,
        DisableAdminLockout = 0x02,
        DisableForceGuest = 0x04,
        AllowOldPassword = 0x08,
        TryCacheFirst = 0x10
    };

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

    typedef struct _CHANGE_PASSWORD_REQUEST {
        PROTOCOL_MESSAGE_TYPE MessageType{ PROTOCOL_MESSAGE_TYPE::ChangePassword };
        UNICODE_STRING DomainName;
        UNICODE_STRING AccountName;
        UNICODE_STRING OldPassword;
        UNICODE_STRING NewPassword;
        BOOLEAN Impersonating;
    } CHANGE_PASSWORD_REQUEST, *PCHANGE_PASSWORD_REQUEST;

    typedef struct _CHANGE_PASSWORD_RESPONSE {
        PROTOCOL_MESSAGE_TYPE MessageType;
        BOOLEAN PasswordInfoValid;
        DOMAIN_PASSWORD_INFORMATION DomainPasswordInfo;
    } CHANGE_PASSWORD_RESPONSE, *PCHANGE_PASSWORD_RESPONSE;

    typedef struct _CHANGE_CACHED_PASSWORD_REQUEST : public CHANGE_PASSWORD_REQUEST {
        _CHANGE_CACHED_PASSWORD_REQUEST() {
            this->MessageType = PROTOCOL_MESSAGE_TYPE::ChangeCachedPassword;
        }
    } CHANGE_CACHED_PASSWORD_REQUEST, *PCHANGE_CACHED_PASSWORD_REQUEST;

    typedef struct _CHANGE_CACHED_PASSWORD_RESPONSE : CHANGE_PASSWORD_RESPONSE {
    } CHANGE_CACHED_PASSWORD_RESPONSE, *PCHANGE_CACHED_PASSWORD_RESPONSE;

    typedef struct _CLEAR_CACHED_CREDENTIALS_REQUEST {
        PROTOCOL_MESSAGE_TYPE MessageType{ PROTOCOL_MESSAGE_TYPE::ClearCachedCredentials };
    } CLEAR_CACHED_CREDENTIALS_REQUEST, *PCLEAR_CACHED_CREDENTIALS_REQUEST;

    typedef struct _DECRYPT_DPAPI_MASTER_KEY_REQUEST {
        PROTOCOL_MESSAGE_TYPE MessageType{ PROTOCOL_MESSAGE_TYPE::DecryptDpapiMasterKey };
        // OWF password type. 0 for NtOwf, 1 for ShaOwf
        // Based off  NtlmCredIsoInProc::DecryptDpapiMasterKey
        DWORD IsShaPassword; // confirmed
        LUID LogonSession;
        DWORD MasterKeyIn;
        DWORD unknown3;
        DWORD unknown4; // used for something
        DWORD unknown5[7];
    } DECRYPT_DPAPI_MASTER_KEY_REQUEST, *PDECRYPT_DPAPI_MASTER_KEY_REQUEST;

    typedef struct _DECRYPT_DPAPI_MASTER_KEY_RESPONSE {
        PROTOCOL_MESSAGE_TYPE MessageType;
        DWORD Reserved;
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
        // Will be 0x28
        DWORD DataLength;
        // The NT and SHA OWF passwords or the DPAPI key
        UCHAR CredentialData[1];
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
        DWORD unknown1; // 0X04
        LUID unknown2; // 0x28
        DWORD unknown3; // 0x30
        DWORD unknown4; // 0x34
        DWORD unknown5; // 0x38
        DWORD unknown6; // 0x40
        GUID unknown7; // 0x48
    } GET_STRONG_CREDENTIAL_KEY_REQUEST, *PGET_STRONG_CREDENTIAL_KEY_REQUEST;

    typedef struct _GET_STRONG_CREDENTIAL_KEY_RESPONSE {
        PROTOCOL_MESSAGE_TYPE MessageType;
        // ...
    } GET_STRONG_CREDENTIAL_KEY_RESPONSE, *PGET_STRONG_CREDENTIAL_KEY_RESPONSE;

    typedef struct _LM20_CHALLENGE_REQUEST {
        PROTOCOL_MESSAGE_TYPE MessageType{ PROTOCOL_MESSAGE_TYPE::Lm20ChallengeRequest };
    } LM20_CHALLENGE_REQUEST, *PLM20_CHALLENGE_REQUEST;

    typedef struct _LM20_CHALLENGE_RESPONSE {
        PROTOCOL_MESSAGE_TYPE MessageType;
        UCHAR ChallengeToClient[8]; // challenge length
    } LM20_CHALLENGE_RESPONSE, *PLM20_CHALLENGE_RESPONSE;

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

    typedef struct _TRANSFER_CRED_REQUEST {
        PROTOCOL_MESSAGE_TYPE MessageType{ PROTOCOL_MESSAGE_TYPE::TransferCred };
        LUID SourceLuid;
        LUID DestinationLuid;
    } TRANSFER_CRED_REQUEST, *PTRANSFER_CRED_REQUEST;

    class Proxy {
    public:
        Proxy(const std::shared_ptr<Lsa>& lsa);

        // A subset of the supported functions in msv1_0
        bool CacheLogon(void* logonInfo, void* validationInfo, const std::vector<byte>& supplementalCacheData, ULONG flags) const;
        bool CacheLookupEx(const std::wstring username, const std::wstring domain, CacheLookupCredType type, const std::string credential) const;
        bool ChangeCachedPassword(const std::wstring& domainName, const std::wstring& accountName, const std::wstring& oldPassword, const std::wstring& newPassword, bool impersonating) const;
        bool ClearCachedCredentials() const;
        bool DecryptDpapiMasterKey() const;
        bool DeleteTbalSecrets() const;
        bool DeriveCredential(PLUID luid, DeriveCredType type, const std::vector<byte>& mixingBits) const;
        bool EnumerateUsers(bool passthrough = false) const;
        bool GenericPassthrough(const std::wstring& domainName, const std::wstring& packageName, std::vector<byte>& data) const;
        bool GetCredentialKey(PLUID luid) const;
        bool GetStrongCredentialKey() const;
        bool GetUserInfo(PLUID luid) const;
        bool Lm20ChallengeRequest() const;
        bool ProvisionTbal(PLUID luid) const;
        bool SetProcessOption(ProcessOption options, bool disable) const;
        bool TransferCred(PLUID sourceLuid, PLUID destinationLuid) const;

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

    bool HandleFunction(std::ostream& out, const Proxy& proxy, const std::string& function, const cxxopts::ParseResult& options);
    void Parse(std::ostream& out, const std::vector<std::string>& args);

    namespace Cache {
        std::unique_ptr<Netlogon::INTERACTIVE_INFO> GetLogonInfo(const std::wstring& domainName, const std::wstring& userName, std::wstring& computerName, const std::vector<byte>& hash, ULONG logonType = RPC_C_AUTHN_GSS_KERBEROS);

        // The GetSupplementalMitCreds function is only provided for convenience
        // Other supplemental cred formats are left to the user to build
        std::vector<byte> GetSupplementalMitCreds(const std::wstring& domainName, const std::wstring& upn);

        // The validationInfo argument is specified as VALIDATION_SAM_INFO3 because may store resource group information
        std::unique_ptr<Netlogon::VALIDATION_SAM_INFO4> GetValidationInfo(Netlogon::PVALIDATION_SAM_INFO3 validationInfo, std::wstring* dnsDomainName);
    }
}