// Copyright (c) 2025 Xsolla Inc. All Rights Reserved.
// This is licensed software from Xsolla Inc. Powered by AccelByte.
// For limitation and restriction, contact your company contract manager.

#pragma once

#include "Core/AccelByteInstance.h"
#include "Core/AccelByteApiClient.h"
#include "Core/AccelByteServerApiClient.h"
#include "OnlineSubsystemAccelByte.h"

namespace Xsolla = AccelByte;
using namespace Xsolla;

#define DEFINE_CLASS_ALIAS(Name, AliasName) \
using AliasName = Name; \
using AliasName##Ptr = TSharedPtr<AliasName, ESPMode::ThreadSafe>;

// SDK
DEFINE_CLASS_ALIAS(FAccelByteInstance, FXsollaSdkInstance)

// OSS
DEFINE_CLASS_ALIAS(FOnlineSubsystemAccelByte, FOnlineSubsystemXsolla)

DEFINE_CLASS_ALIAS(FOnlineAchievementsAccelByte, FOnlineAchievementsXsolla)
DEFINE_CLASS_ALIAS(FOnlineAgreementAccelByte, FOnlineAgreementXsolla)
DEFINE_CLASS_ALIAS(FOnlineAnalyticsAccelByte, FOnlineAnalyticsXsolla)
DEFINE_CLASS_ALIAS(FOnlineAuthAccelByte, FOnlineAuthXsolla)
DEFINE_CLASS_ALIAS(FOnlineChatAccelByte, FOnlineChatXsolla)
DEFINE_CLASS_ALIAS(FOnlineCloudSaveAccelByte, FOnlineCloudSaveXsolla)
DEFINE_CLASS_ALIAS(FOnlineEntitlementsAccelByte, FOnlineEntitlementsXsolla)
DEFINE_CLASS_ALIAS(FOnlineExternalUIAccelByte, FOnlineExternalUIXsolla)
DEFINE_CLASS_ALIAS(FOnlineFriendsAccelByte, FOnlineFriendsXsolla)
DEFINE_CLASS_ALIAS(FOnlineGameStandardEventAccelByte, FOnlineGameStandardEventXsolla)
DEFINE_CLASS_ALIAS(FOnlineGroupsAccelByte, FOnlineGroupsXsolla)
DEFINE_CLASS_ALIAS(FOnlineIdentityAccelByte, FOnlineIdentityXsolla)
DEFINE_CLASS_ALIAS(FOnlineLeaderboardAccelByte, FOnlineLeaderboardXsolla)
DEFINE_CLASS_ALIAS(FOnlinePredefinedEventAccelByte, FOnlinePredefinedEventXsolla)
DEFINE_CLASS_ALIAS(FOnlinePresenceAccelByte, FOnlinePresenceXsolla)
DEFINE_CLASS_ALIAS(FOnlinePurchaseAccelByte, FOnlinePurchaseXsolla)
DEFINE_CLASS_ALIAS(FOnlineSessionV2AccelByte, FOnlineSessionXsolla)
DEFINE_CLASS_ALIAS(FOnlineStatisticAccelByte, FOnlineStatisticXsolla)
DEFINE_CLASS_ALIAS(FOnlineStoreV2AccelByte, FOnlineStoreXsolla)
DEFINE_CLASS_ALIAS(FOnlineTimeAccelByte, FOnlineTimeXsolla)
DEFINE_CLASS_ALIAS(FOnlineUserAccelByte, FOnlineUserXsolla)
DEFINE_CLASS_ALIAS(FOnlineUserCacheAccelByte, FOnlineUserCacheXsolla)
DEFINE_CLASS_ALIAS(FOnlineUserCloudAccelByte, FOnlineUserCloudXsolla)
DEFINE_CLASS_ALIAS(FOnlineVoiceAccelByte, FOnlineVoiceXsolla)
DEFINE_CLASS_ALIAS(FOnlineWalletAccelByte, FOnlineWalletXsolla)
