# Xsolla Game Services Backend
This plugin is backend sdk for your gaming service, enabling it to work seamlessly with the Xsolla store-ue4-sdk.

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

### AccelByte-named runtime keys (do not rename)

The following runtime keys **must** keep their AccelByte names. Renaming them
will break login and online-subsystem look-up:

| Key | Purpose |
|---|---|
| `[OnlineSubsystemAccelByte]` | OSS configuration section |
| `DefaultPlatformService=AccelByte` (in `[OnlineSubsystem]`) | Engine subsystem identifier |

These are internal engine identifiers; the Xsolla wrapper leaves them as-is.
