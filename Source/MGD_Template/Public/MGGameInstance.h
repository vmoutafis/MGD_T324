// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Interfaces/OnlineIdentityInterface.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "OnlineSubsystem.h"

#include "MGGameInstance.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHostGameComplete, bool, success);

/**
 * 
 */
UCLASS()
class MGD_TEMPLATE_API UMGGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	virtual void Init() override;
	
	UFUNCTION(BlueprintCallable, Category=Login)
	void LoginToEOS();

	UFUNCTION(BlueprintCallable, Category=Session)
	void HostGame();

	UFUNCTION(BlueprintCallable, Category=Session)
	bool StartGame();
	
	UFUNCTION(BlueprintPure, Category=Login)
	bool IsLoggedIn() const;

	UFUNCTION(BlueprintPure, Category=Session)
	bool IsInSession() const;

	UFUNCTION(BlueprintPure, Category=Login)
	FString GetPlayerNickname() const;

	UPROPERTY(BlueprintAssignable, Category=Session)
	FOnHostGameComplete Delegate_OnHostGameComplete;

protected:
	UFUNCTION(BlueprintImplementableEvent, Category=Login)
	void BIEOnLoginComplete(bool success, const FString& error);
	
	// the login function that will run when the onlogincomplete delegate is run
	void OnLoginComplete(int32 localUserNum, bool bWasSuccessful, const FUniqueNetId& UserId, const FString& Error);

	void OnCreateSessionComplete(FName sessionName, bool success);
	void OnInviteAccepted(const bool bWasSuccessful, const int32 ControllerId, FUniqueNetIdPtr UserId, const FOnlineSessionSearchResult& InviteResult);
	void OnSessionJoinComplete(FName sessionName, EOnJoinSessionCompleteResult::Type joinResult);
	
	// this will store a reference to the online subsystem
	const IOnlineSubsystem* OssRef;

	// this will store a reference to the identity interface in the online subsystem
	IOnlineIdentityPtr IdentityInterface;
	
	IOnlineSessionPtr SessionInterface;

	UPROPERTY(EditDefaultsOnly, Category=Maps)
	TSoftObjectPtr<UWorld> GameMap;

};
