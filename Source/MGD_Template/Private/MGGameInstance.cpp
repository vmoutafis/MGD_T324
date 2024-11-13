// Fill out your copyright notice in the Description page of Project Settings.


#include "MGGameInstance.h"
#include "OnlineSubsystemUtils.h"
#include "OnlineSessionSettings.h"
#include "GameFramework/GameModeBase.h"
#include "Online/OnlineSessionNames.h"

void UMGGameInstance::Init()
{
	Super::Init();

	// store the online subsystem
	OssRef = Online::GetSubsystem(GetWorld());

	// check if it was found and error if not
	if (!OssRef)
	{
		UE_LOG(LogTemp, Error, TEXT("Online subsystem not found."))
		return;
	}

	// store the identity interface from the oss
	IdentityInterface = OssRef->GetIdentityInterface();

	// check if the identity interface was found, error if not
	if (!IdentityInterface)
	{
		UE_LOG(LogTemp, Error, TEXT("Identity interface not found."))
		return;
	}

	// bind to the login complete delegate in the identity interface
	IdentityInterface->OnLoginCompleteDelegates->AddUObject(this, &UMGGameInstance::OnLoginComplete);

	SessionInterface = OssRef->GetSessionInterface();

	if (!SessionInterface)
	{
		UE_LOG(LogTemp, Error, TEXT("Session interface not found."))
		return;
	}

	SessionInterface->OnCreateSessionCompleteDelegates.AddUObject(this, &UMGGameInstance::OnCreateSessionComplete);
	SessionInterface->OnSessionUserInviteAcceptedDelegates.AddUObject(this, &UMGGameInstance::OnInviteAccepted);
	SessionInterface->OnJoinSessionCompleteDelegates.AddUObject(this, &UMGGameInstance::OnSessionJoinComplete);
}

void UMGGameInstance::LoginToEOS()
{
	if (!IdentityInterface)
	{
		UE_LOG(LogTemp, Error, TEXT("Cannot login, identity interface not found."))
		return;
	}

	FOnlineAccountCredentials creds;
	creds.Type = TEXT("accountportal");
	
	if (!IdentityInterface->Login(0, creds))
	{
		UE_LOG(LogTemp, Error, TEXT("Login failed synchronously"))
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("Attempting to login..."))
}

void UMGGameInstance::HostGame()
{
	if (!IsLoggedIn() || !SessionInterface)
		return;

	FOnlineSessionSettings sessionSettings;

	sessionSettings.bAllowInvites = true;
	sessionSettings.bIsDedicated = false;
	sessionSettings.bUsesPresence = true;
	sessionSettings.NumPrivateConnections = 4;
	sessionSettings.bIsLANMatch = false;
	sessionSettings.bUseLobbiesIfAvailable = true;

	if (!SessionInterface->CreateSession(0, TEXT("MGSESSION"), sessionSettings))
	{
		UE_LOG(LogTemp, Error, TEXT("Session failed to create synchronously"))
		return;
	}

	UE_LOG(LogTemp, Error, TEXT("Attempting to host session..."))
}

bool UMGGameInstance::StartGame()
{
	if (GameMap.IsNull())
		return false;
	
	GetWorld()->GetAuthGameMode()->bUseSeamlessTravel = true;

	if (!GetWorld()->ServerTravel("/Game/MyContent/Maps/Lvl_Test?listen?GameMode=/Script/MGD_Template.GM_Battle"))
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to travel to map"))
		return false;
	}

	UE_LOG(LogTemp, Warning, TEXT("Travelling to map"))
	return true;
}

bool UMGGameInstance::IsLoggedIn() const
{
	if (!IdentityInterface)
		return false;

	return IdentityInterface->GetLoginStatus(0) == ELoginStatus::LoggedIn;
}

bool UMGGameInstance::IsInSession() const
{
	if (!SessionInterface)
		return false;

	return SessionInterface->GetSessionState(TEXT("MGSESSION")) != EOnlineSessionState::NoSession;
}

FString UMGGameInstance::GetPlayerNickname() const
{
	if (!IsLoggedIn())
		return TEXT("NOT LOGGED IN");

	return IdentityInterface->GetPlayerNickname(0);
}

void UMGGameInstance::OnLoginComplete(int32 localUserNum, bool bWasSuccessful, const FUniqueNetId& UserId,
                                      const FString& Error)
{
	BIEOnLoginComplete(bWasSuccessful, Error);
}

void UMGGameInstance::OnCreateSessionComplete(FName sessionName, bool success)
{
	if (!EnableListenServer(true))
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to create listen server"))
		SessionInterface->EndSession(sessionName);
		success = false;
	}

	Delegate_OnHostGameComplete.Broadcast(success);

	if (!success)
	{
		UE_LOG(LogTemp, Warning, TEXT("Session failed to create"))
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("Successfully hosted session"))
}

void UMGGameInstance::OnInviteAccepted(const bool bWasSuccessful, const int32 ControllerId, FUniqueNetIdPtr UserId,
                                       const FOnlineSessionSearchResult& InviteResult)
{
	if (!bWasSuccessful)
	{
		UE_LOG(LogTemp, Error, TEXT("Invite failed to accept"))
		return;
	}
	
	if (!SessionInterface)
		return;

	SessionInterface->JoinSession(0, TEXT("MGSESSION"), InviteResult);
}

void UMGGameInstance::OnSessionJoinComplete(FName sessionName, EOnJoinSessionCompleteResult::Type joinResult)
{
	if (joinResult != EOnJoinSessionCompleteResult::Success)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to join session"))
		return;
	}

	if (!SessionInterface)
		return;

	ClientTravelToSession(0, sessionName);
}
