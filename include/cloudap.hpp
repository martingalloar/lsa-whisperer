#pragma once
#include <pch.hpp>

#include <lsa.hpp>
#include <memory>
#include <string>

#define CLOUDAP_NAME_A "cloudap"

namespace Cloudap {
    enum class PROTOCOL_MESSAGE_TYPE : ULONG {
        ReinitPlugin = 0,
        GetTokenBlob,
        CallPluginGeneric,
        ProfileDeleted,
        GetAuthenticatingProvider,
        RenameAccount,
        RefreshTokenBlob,
        GenARSOPwd,
        SetTestParas,
        TransferCreds,
        ProvisionNGCNode,
        GetPwdExpiryInfo,
        DisableOptimizedLogon,
        GetUnlockKeyType,
        GetPublicCachedInfo,
        GetAccountInfo,
        GetDpApiCredKeyDecryptStatus,
        IsCloudToOnPremTgtPresentInCache
    };

    enum class TEST_FLAG : ULONG {
        EnableIdentityCacheFlushes = 1,
        EnablePreRS2Support = 2
    };

    typedef struct _CALL_PLUGIN_GENERIC_REQUEST {
        PROTOCOL_MESSAGE_TYPE MessageType{ PROTOCOL_MESSAGE_TYPE::CallPluginGeneric };
        GUID Package;
        ULONG BufferLength;
        CHAR Buffer[ANYSIZE_ARRAY];
    } CALL_PLUGIN_GENERIC_REQUEST, *PCALL_PLUGIN_GENERIC_REQUEST;

    typedef struct _DISABLE_OPTIMIZED_LOGON_REQUEST {
        PROTOCOL_MESSAGE_TYPE MessageType{ PROTOCOL_MESSAGE_TYPE::DisableOptimizedLogon };
        LUID Luid{ 0 };
    } DISABLE_OPTIMIZED_LOGON_REQUEST, *PDISABLE_OPTIMIZED_LOGON_REQUEST;

    typedef struct _GEN_ARSO_PASSWORD_REQUEST {
        PROTOCOL_MESSAGE_TYPE MessageType{ PROTOCOL_MESSAGE_TYPE::GenARSOPwd };
        LUID Luid{ 0 };
        ULONG BufferLength;
        CHAR Buffer[0];
    } GEN_ARSO_PASSWORD_REQUEST, *PGEN_ARSO_PASSWORD_REQUEST;

    typedef struct _GET_ACCOUNT_INFO_REQUEST {
        PROTOCOL_MESSAGE_TYPE MessageType{ PROTOCOL_MESSAGE_TYPE::GenARSOPwd };
        GUID PackageGuid{ 0 };
        PSID Sid;
        // Pad of 0x3C
    } GET_ACCOUNT_INFO_REQUEST, *PGET_ACCOUNT_INFO_REQUEST;

    typedef struct _GET_AUTHENTICATION_PROVIDER_REQUEST {
        PROTOCOL_MESSAGE_TYPE MessageType{ PROTOCOL_MESSAGE_TYPE::GetAuthenticatingProvider };
        LUID Luid{ 0 };
    } GET_AUTHENTICATION_PROVIDER_REQUEST, *PGET_AUTHENTICATION_PROVIDER_REQUEST;

    typedef struct _GET_AUTHENTICATION_PROVIDER_RESPONSE {
        GUID provider;
    } GET_AUTHENTICATION_PROVIDER_RESPONSE, *PGET_AUTHENTICATION_PROVIDER_RESPONSE;
    
    typedef struct _GET_DP_API_CRED_KEY_DECRYPT_STATUS_REQUEST {
        PROTOCOL_MESSAGE_TYPE MessageType{ PROTOCOL_MESSAGE_TYPE::GetDpApiCredKeyDecryptStatus };
        LUID Luid{ 0 };
    } GET_DP_API_CRED_KEY_DECRYPT_STATUS_REQUEST, *PGET_DP_API_CRED_KEY_DECRYPT_STATUS_REQUEST;

    typedef struct _GET_DP_API_CRED_KEY_DECRYPT_STATUS_RESPONSE {
        DWORD IsDecrypted;
    } GET_DP_API_CRED_KEY_DECRYPT_STATUS_RESPONSE, *PGET_DP_API_CRED_KEY_DECRYPT_STATUS_RESPONSE;

    typedef struct _GET_PUBLIC_CACHED_INFO_REQUEST { // wip
        // code + package guid + uint 6 + (uint + uint) + (uint + uint).
        PROTOCOL_MESSAGE_TYPE MessageType{ PROTOCOL_MESSAGE_TYPE::GenARSOPwd };
        GUID PackageGuid{ 0 };
        ULONG StringLength{ 6 }; // Length must be 6
        ULONG StringMaximumLength;
        PWSTR StringBuffer;
    } GET_PUBLIC_CACHED_INFO_REQUEST, *PGET_PUBLIC_CACHED_INFO_REQUEST;

    typedef struct _GET_PWD_EXPIRY_INFO_REQUEST {
        PROTOCOL_MESSAGE_TYPE MessageType{ PROTOCOL_MESSAGE_TYPE::GetPwdExpiryInfo };
        LUID Luid{ 0 };
    } GET_PWD_EXPIRY_INFO_REQUEST, *PGET_PWD_EXPIRY_INFO_REQUEST;

    typedef struct _GET_PWD_EXPIRY_INFO_RESPONSE {
        FILETIME PwdExpirationTime;
        WCHAR PwdResetUrl[0];
    } GET_PWD_EXPIRY_INFO_RESPONSE, *PGET_PWD_EXPIRY_INFO_RESPONSE;

    typedef struct _GET_TOKEN_BLOB_REQUEST {
        PROTOCOL_MESSAGE_TYPE MessageType{ PROTOCOL_MESSAGE_TYPE::GetTokenBlob };
        LUID Luid{ 0 };
    } GET_TOKEN_BLOB_REQUEST, *PGET_TOKEN_BLOB_REQUEST;

    typedef struct _GET_UNLOCK_KEY_TYPE_REQUEST {
        PROTOCOL_MESSAGE_TYPE MessageType{ PROTOCOL_MESSAGE_TYPE::GetUnlockKeyType };
        LUID Luid{ 0 };
    } GET_UNLOCK_KEY_TYPE_REQUEST, *PGET_UNLOCK_KEY_TYPE_REQUEST;

    typedef struct _GET_UNLOCK_KEY_TYPE_RESPONSE {
        DWORD Type;
    } GET_UNLOCK_KEY_TYPE_RESPONSE, *PGET_UNLOCK_KEY_TYPE_RESPONSE;

    typedef struct _IS_CLOUD_TO_ON_PREM_TGT_PRESENT_IN_CACHE_REQUEST {
        PROTOCOL_MESSAGE_TYPE MessageType{ PROTOCOL_MESSAGE_TYPE::IsCloudToOnPremTgtPresentInCache };
        LUID Luid{ 0 };
    } IS_CLOUD_TO_ON_PREM_TGT_PRESENT_IN_CACHE_REQUEST, *PIS_CLOUD_TO_ON_PREM_TGT_PRESENT_IN_CACHE_REQUEST;

    typedef struct _IS_CLOUD_TO_ON_PREM_TGT_PRESENT_IN_CACHE_RESPONSE {
        DWORD IsPresent;
    } IS_CLOUD_TO_ON_PREM_TGT_PRESENT_IN_CACHE_RESPONSE, *PIS_CLOUD_TO_ON_PREM_TGT_PRESENT_IN_CACHE_RESPONSE;

    typedef struct _PROFILE_DELETED_REQUEST {
        PROTOCOL_MESSAGE_TYPE MessageType{ PROTOCOL_MESSAGE_TYPE::ProfileDeleted };
        ULONG SidOffset; // Offset to PSID pointer within structure
        ULONG SidLength;
        ULONG SidMaximumLength;
        PSID Sid;
    } PROFILE_DELETED_REQUEST, *PPROFILE_DELETED_REQUEST;

    typedef struct _SET_TEST_PARAS_REQUEST {
        PROTOCOL_MESSAGE_TYPE MessageType{ PROTOCOL_MESSAGE_TYPE::SetTestParas };
        ULONG Flags{ 0 };
    } SET_TEST_PARAS_REQUEST, *PSET_TEST_PARAS_REQUEST;

    class Proxy {
    public:
        Proxy(const std::shared_ptr<Lsa>& lsa);

        // Supported functions in cloudAP!PluginFunctionTable
        bool CallPluginGeneric(const GUID* plugin, const std::string& json, void** returnBuffer, size_t* returnBufferLength) const;
        bool DisableOptimizedLogon(PLUID luid) const;
        bool GenARSOPwd(PLUID luid, const std::string& data) const;
        bool GetAccountInfo() const;
        bool GetAuthenticatingProvider(PLUID luid) const;
        bool GetDpApiCredKeyDecryptStatus(PLUID luid) const;
        bool GetPublicCachedInfo() const;
        bool GetPwdExpiryInfo(PLUID luid) const;
        bool GetTokenBlob(PLUID luid) const;
        bool GetUnlockKeyType(PLUID luid) const;
        bool IsCloudToOnPremTgtPresentInCache(PLUID luid) const;
        bool ProfileDeleted() const;
        bool ProvisionNGCNode() const;
        bool RefreshTokenBlob() const;
        bool ReinitPlugin() const;
        bool RenameAccount() const;
        bool SetTestParas(ULONG TestFlags) const;
        bool TransferCreds(PLUID sourceLuid, PLUID destinationLuid) const;

    protected:
        std::shared_ptr<Lsa> lsa;

        bool CallPackage(const std::string& submitBuffer, void** returnBuffer) const;

        bool CallPackage(PROTOCOL_MESSAGE_TYPE MessageType) const;

        // You must free all returnBuffer outputs with LsaFreeReturnBuffer
        template<typename _Request, typename _Response>
        bool CallPackage(const _Request& submitBuffer, _Response** returnBuffer) const;

        template<typename _Request, typename _Response>
        bool CallPackage(const _Request& submitBuffer, _Response** returnBuffer, size_t* returnBufferLength) const;
    };

    // The AzureAD plugin (AAD), implemented in aadcloudap.dll
    namespace Aad {
        typedef enum _AUTHORITY_TYPE {
            AzureAd = 1,
            Enterprise = 2,
        } AUTHORITY_TYPE;

        enum class CALL : ULONG {
            SignPayload = 1,
            CreateSSOCookie,
            GetPrtAuthority,
            CheckDeviceKeysHealth,
            DeviceAuth,
            RefreshP2PCACert,
            DeviceValidityCheck,
            CreateDeviceSSOCookie,
            CreateNonce,
            ValidateRdpAssertionRequest,
            RefreshP2PCerts,
            CreateBindingKey,
            GenerateBindingClaims,
            CreateEnterpriseSSOCookie = 0xf,
        };

        // {B16898C6-A148-4967-9171-64D755DA8520}
        extern GUID AadGlobalIdProviderGuid;

        class Proxy : public Cloudap::Proxy {
        public:
            Proxy(const std::shared_ptr<Lsa>& lsa);

            bool CheckDeviceKeysHealth() const;
            bool CreateBindingKey() const;
            bool CreateDeviceSSOCookie(const std::string& server, const std::string& nonce) const;
            bool CreateEnterpriseSSOCookie(const std::string& server, const std::string& nonce) const;
            bool CreateNonce() const;
            bool CreateSSOCookie(const std::string& server, const std::string& nonce) const;
            bool DeviceAuth() const;
            bool DeviceValidityCheck() const;
            bool GenerateBindingClaims() const;
            bool GetPrtAuthority(AUTHORITY_TYPE authority) const;
            bool RefreshP2PCACert() const;
            bool RefreshP2PCerts() const;
            bool SignPayload() const;
            bool ValidateRdpAssertionRequest(const std::string& authenticationRequest) const;
        };
    }

    // The MicrosoftAccount plugin (MSA), implemented in MicrosoftAccountCloudAP.dll
    namespace Msa {
        // {D7F9888F-E3FC-49b0-9EA6-A85B5F392A4F}
        extern GUID WLIDProviderGuid;
    }
}