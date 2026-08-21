// Copyright (c) 2025 Xsolla Inc. All Rights Reserved.
// This is licensed software from Xsolla Inc. Powered by AccelByte.
// For limitation and restriction, contact your company contract manager.

#include "Api/XsollaAccelByteAuthCallbackProxy.h"

void UXsollaAccelByteAuthCallbackProxy::HandleAuthUpdate(const FXsollaLoginData& LoginData)
{
    if (OnAuthUpdate)
    {
        OnAuthUpdate(LoginData);
    }
}

void UXsollaAccelByteAuthCallbackProxy::HandleAuthError(const FString& Code, const FString& Description)
{
    if (OnAuthError)
    {
        OnAuthError(Code, Description);
    }
}

void UXsollaAccelByteAuthCallbackProxy::HandleAuthCancel()
{
    if (OnAuthCancel)
    {
        OnAuthCancel();
    }
}
