// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "OnlineSubsystem.h"
#include "Interfaces/OnlineSessionInterface.h"

#include "MGGameInstance.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnMGLoginComplete, bool, success, const FString, userName);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMGSessionCreated, bool, success);

/**
 * 
 */
UCLASS()
class MGD_TEMPLATE_API UMGGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	virtual void Init() override;
	
	UFUNCTION(BlueprintCallable, Category=Online)
	void LoginToEOS();

	UFUNCTION(BlueprintPure, Category=Online)
	bool IsLoggedIn() const;

	UPROPERTY(BlueprintAssignable, Category=Online)
	FOnMGLoginComplete Delegate_OnLoginComplete;

	UPROPERTY(BlueprintAssignable, Category=Online)
	FOnMGSessionCreated Delegate_OnMgSessionCreated;

	UFUNCTION(BlueprintPure, Category=Login)
	FString GetPlayerNickname() const;

	UFUNCTION(BlueprintCallable, Category=Online)
	void HostSession();

	UFUNCTION(BlueprintPure, Category=Online)
	bool IsInSession() const;

protected:
	// Login delegate binds //
	
	void OnLoginComplete(int32 localUserNum, bool bWasSuccessful, const FUniqueNetId& UserId, const FString& Error);

	// Session delegate binds //

	void OnSessionCreated(FName sessionName, bool success);
	void OnSessionInviteAccepted(const bool bWasSuccessful, const int32 ControllerId, FUniqueNetIdPtr UserId, const FOnlineSessionSearchResult& InviteResult);
	void OnSessionJoined(FName sessionName, EOnJoinSessionCompleteResult::Type result);
	
	const IOnlineSubsystem* OSSRef;

	IOnlineIdentityPtr IdentityInterface;

	IOnlineSessionPtr SessionInterface;

	FUniqueNetIdPtr UniqueUserId;
	
};
