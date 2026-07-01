# Xsolla Game Services Backend

## Overview
Backend SDK for your gaming service, enabling it to work seamlessly with the Xsolla `store-ue4-sdk`. All credentials, base URLs, and runtime behavior are configured through a single Xsolla settings class (`UXsollaSettings`) — see [Configuration](#configuration).

This is a distribution repo, not a game project — there is no `.uproject` or build harness here. To use or verify it, copy/symlink the plugins into a UE project's `Plugins/` folder and build there.

## Supported Unreal Engine
Target: Unreal Engine 5.7.

## Dependencies
This bundle includes the following plugins (bundled as submodules of this repo):
1. `XsollaBackendSdk` — the wrapper SDK; the only first-party code in this repo.
2. `AccelByteUe4Sdk`
3. `OnlineSubsystemAccelByte`
4. `AccelByteNetworkUtilities`

## Configuration

All credentials and base URLs are configured through a single Xsolla settings
class (`UXsollaSettings`).

**DefaultEngine.ini section:**

```ini
[/Script/XsollaBackendSdk.XsollaSettings]
ClientId=<your client id>
Namespace=<game namespace>
PublisherNamespace=<publisher namespace>
RedirectURI=<redirect uri>
BaseUrl=https://example.gamingservices.xsolla.com
ServerClientId=<server client id>
ServerClientSecret=<server client secret>
ServerNamespace=<server namespace>
ServerPublisherNamespace=<server publisher namespace>
ServerBaseUrl=https://example.gamingservices.xsolla.com
```

In the Unreal Editor the same settings appear under
**Project Settings -> Plugins -> Xsolla Backend SDK**.

### Internal runtime keys (do not rename)

The following runtime keys **must** keep their exact names. Renaming them
will break login and online-subsystem look-up:

| Key | Purpose |
|---|---|
| `[OnlineSubsystemAccelByte]` | OSS configuration section |
| `DefaultPlatformService=AccelByte` (in `[OnlineSubsystem]`) | Engine subsystem identifier |

These are internal engine identifiers; the Xsolla wrapper leaves them as-is.
