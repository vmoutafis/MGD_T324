// Fill out your copyright notice in the Description page of Project Settings.


#include "MGGameInstance.h"

#include "OnlineSubsystemUtils.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "OnlineSessionSettings.h"
#include "Online/OnlineSessionNames.h"

void UMGGameInstance::Init()
{
	Super::Init();

	OSSRef = Online::GetSubsystem(GetWorld());

	if (!OSSRef)
	{
		UE_LOG(LogTemp, Error, TEXT("No online subsystem found"))
		return;
	}

	IdentityInterface = OSSRef->GetIdentityInterface();

	if (!IdentityInterface)
	{
		UE_LOG(LogTemp, Error, TEXT("No identity interface found"))
		return;
	}

	// bind to the on login complete event
	IdentityInterface->OnLoginCompleteDelegates->AddUObject(this, &UMGGameInstance::OnLoginComplete);

	SessionInterface = OSSRef->GetSessionInterface();

	if (!SessionInterface)
	{
		UE_LOG(LogTemp, Error, TEXT("No session interface found"))
		return;
	}

	// bind to the on create session complete event
	SessionInterface->OnCreateSessionCompleteDelegates.AddUObject(this, &UMGGameInstance::OnSessionCreated);
	
	// bind to the on invited accepted event
	SessionInterface->OnSessionUserInviteAcceptedDelegates.AddUObject(this, &UMGGameInstance::OnSessionInviteAccepted);

	// bind to the on joined session delegate
	SessionInterface->OnJoinSessionCompleteDelegates.AddUObject(this, &UMGGameInstance::OnSessionJoined);
}

void UMGGameInstance::LoginToEOS()
{
	if (!IdentityInterface)
		return;

	FOnlineAccountCredentials creds;
	creds.Type = FString(TEXT("accountportal"));

	if (IdentityInterface->Login(0, creds))
	{
		UE_LOG(LogTemp, Warning, TEXT("Attempting to login to EOS"))
	}
}

bool UMGGameInstance::IsLoggedIn() const
{
	if (!IdentityInterface)
		return false;

	return IdentityInterface->GetLoginStatus(0) == ELoginStatus::LoggedIn;
}

FString UMGGameInstance::GetPlayerNickname() const
{
	if (!IsLoggedIn())
		return TEXT("USER NOT LOGGED IN");

	return IdentityInterface->GetPlayerNickname(0);
}

void UMGGameInstance::HostSession()
{
	if (!IsLoggedIn())
		return;

	if (!SessionInterface)
		return;

	if (IsInSession())
	{
		UE_LOG(LogTemp, Warning, TEXT("Already connected to a session"))
		return;
	}

	FOnlineSessionSettings settings;
	settings.NumPrivateConnections = 4;
	settings.bIsLANMatch = false;
	settings.bIsDedicated = false;
	settings.bAllowInvites = true;
	settings.bShouldAdvertise = true;
	settings.bUsesPresence = true;
	settings.bUseLobbiesIfAvailable = true;
	settings.bAllowJoinViaPresence = true;
	settings.Set(SEARCH_LOBBIES, true, EOnlineDataAdvertisementType::ViaOnlineService);
	settings.Set(SEARCH_KEYWORDS, FString(TEXT("MGSESSION")), EOnlineDataAdvertisementType::ViaOnlineService);

	SessionInterface->CreateSession(0, TEXT("MGSESSION"), settings);
}

bool UMGGameInstance::IsInSession() const
{
	if (!SessionInterface)
		return false;
	
	return SessionInterface->GetSessionState(TEXT("MGSESSION")) != EOnlineSessionState::NoSession;
}

void UMGGameInstance::OnLoginComplete(int32 localUserNum, bool bWasSuccessful, const FUniqueNetId& UserId,
                                      const FString& Error)
{
	if (!bWasSuccessful)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to login to EOS - %s"), *Error)
	}
	else
	{
		UniqueUserId = UserId.AsShared();
		UE_LOG(LogTemp, Warning, TEXT("Successfully logged in to EOS"))
	}

	Delegate_OnLoginComplete.Broadcast(bWasSuccessful, GetPlayerNickname());
}

void UMGGameInstance::OnSessionCreated(FName sessionName, bool success)
{
	if (!success)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to create session"))
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Successfully hosting session"))
		EnableListenServer(true);
	}

	Delegate_OnMgSessionCreated.Broadcast(success);
}

void UMGGameInstance::OnSessionInviteAccepted(const bool bWasSuccessful, const int32 ControllerId,
	FUniqueNetIdPtr UserId, const FOnlineSessionSearchResult& InviteResult)
{
	if (!bWasSuccessful || !IsLoggedIn())
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to accept session invite"))
		return;
	}

	SessionInterface->JoinSession(0, TEXT("MGSESSION"), InviteResult);
}

void UMGGameInstance::OnSessionJoined(FName sessionName, EOnJoinSessionCompleteResult::Type result)
{
	if (result != EOnJoinSessionCompleteResult::Success)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to join session %d"), result)
		return;
	}

	ClientTravelToSession(0, TEXT("MGSESSION"));
}
