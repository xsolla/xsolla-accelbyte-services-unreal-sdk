# Xsolla AccelByte SDK

An Unreal Engine plugin that bridges Xsolla authentication into [AccelByte Gaming Services](https://github.com/AccelByte/accelbyte-unreal-sdk-plugin).

This package keeps AccelByte as the game backend SDK and adds a focused Xsolla authentication bridge. It does not replace or mirror the wider AccelByte SDK or Online Subsystem API surface.

## Requirements

- Unreal Engine 5.2 or higher
- `OnlineSubsystemSteam`, only if using Steam silent auth

## Installation

Add this repository as a plugin folder inside your Unreal project:

```text
YourUnrealProject/
  Plugins/
    xsolla-accelbyte-services-unreal-sdk/
      XsollaAccelByteSdk/
      AccelByteUe4Sdk/
      OnlineSubsystemAccelByte/
      AccelByteNetworkUtilities/
```

Clone the repository with submodules, or run this command from the repository root after cloning:

```bash
git submodule update --init --recursive
```

Enable the plugin in your project's `.uproject` file:

```json
{
  "Name": "XsollaAccelByteSdk",
  "Enabled": true
}
```

Add `XsollaAccelByteSdk` to your game module's dependency list in `Source/<YourGame>/<YourGame>.Build.cs`:

```csharp
PublicDependencyModuleNames.AddRange(new string[]
{
    "XsollaAccelByteSdk"
});
```

The package already embeds:

- `AccelByteUe4Sdk` - AccelByte's Unreal SDK for direct AGS API access.
- `OnlineSubsystemAccelByte` - AccelByte's Unreal Online Subsystem integration.
- `AccelByteNetworkUtilities` - AccelByte networking helpers used by the Unreal OSS integration.

Avoid duplicate active copies of the embedded AccelByte plugins in the same Unreal project, because UnrealBuildTool will report duplicate module rule definitions.

## Configuring The SDK

Add the AccelByte SDK and Online Subsystem settings to your project's `Config/DefaultEngine.ini`:

```ini
[OnlineSubsystem]
DefaultPlatformService=AccelByte

[OnlineSubsystemAccelByte]
bEnabled=true

[/Script/AccelByteUe4Sdk.AccelByteSettings]
ClientId=<GAME_CLIENT_ID>
Namespace=<GAME_NAMESPACE>
PublisherNamespace=<PUBLISHER_NAMESPACE>
RedirectURI="http://127.0.0.1"
BaseUrl="<ACCELBYTE_BASE_URL>"

[/Script/AccelByteUe4Sdk.AccelByteServerSettings]
ClientId=<SERVER_CLIENT_ID>
ClientSecret=<SERVER_CLIENT_SECRET>
Namespace=<GAME_NAMESPACE>
PublisherNamespace=<PUBLISHER_NAMESPACE>
RedirectURI="http://127.0.0.1"
BaseUrl="<ACCELBYTE_BASE_URL>"
```

## Usage

Xsolla auth is exposed in two styles, matching however the rest of the project talks to AccelByte - the login methods and delegate types are identical either way, only the accessor differs.

### OSS Style

```cpp
#include "Auth/XsollaAccelByteAuth.h"
#include "XsollaAccelByteSubsystem.h"

UXsollaAccelByteSubsystem* XsollaSubsystem = UXsollaAccelByteSubsystem::Get(this);
UXsollaAccelByteAuth* XsollaAuth = XsollaSubsystem ? XsollaSubsystem->GetAuth() : nullptr;

FOnXsollaAccelByteLoginSuccess OnSuccess;
OnSuccess.BindLambda([](const FXsollaAccelByteLoginResult& LoginResult)
{
    // The player is authenticated with AccelByte through Xsolla.
});

if (XsollaAuth)
{
    XsollaAuth->LoginWithXsollaAccount(OnSuccess);
}
```

### SDK Style

For projects that use the AccelByte SDK directly, without the AccelByte Online Subsystem:

```cpp
#include "Core/AccelByteInstance.h"
#include "Api/XsollaAccelByteAuthApi.h"
#include "AccelByteUe4SdkModule.h"

FAccelByteInstancePtr Instance = IAccelByteUe4SdkModuleInterface::Get().CreateAccelByteInstance();
AccelByte::FApiClientPtr ApiClient = Instance->GetApiClient();

AccelByte::Api::XsollaAccelByteAuthPtr XsollaAuth =
    ApiClient->GetApiPtr<AccelByte::Api::XsollaAccelByteAuth>(GetWorld());

FOnXsollaAccelByteLoginSuccess OnSuccess;
OnSuccess.BindLambda([](const FXsollaAccelByteLoginResult& LoginResult)
{
    // The player is authenticated with AccelByte through Xsolla.
});

if (XsollaAuth.IsValid())
{
    XsollaAuth->LoginWithXsollaAccount(OnSuccess);
}
```

Both also expose `LoginWithXsollaSilentAuth`, `LoginWithXsollaSessionTicket`, and `LoginWithXsollaAccessToken`, and take the same `OnLoginFailed`/`OnLoginCancelled` delegates as `LoginWithXsollaAccount` above. Each auth object allows one active login flow at a time; overlapping calls fail through `OnLoginFailed` with `xsolla-login-in-progress`, while retries after success, failure, or cancellation are allowed.

Two things differ between the styles: OSS style populates `LocalUserNum`/`UserNetId` on `FXsollaAccelByteLoginResult` (it logs in through `IOnlineIdentityPtr`); SDK style doesn't, since it logs in directly via `ApiClient->GetUserApi()` and there's no OSS identity to source those from. And a `UWorld*` is needed either way for `LoginWithXsollaAccount`/`LoginWithXsollaSessionTicket`, since the login widget and session-ticket exchange come from the Xsolla Login plugin's own `UGameInstanceSubsystem`, independent of which AccelByte style you're using.
