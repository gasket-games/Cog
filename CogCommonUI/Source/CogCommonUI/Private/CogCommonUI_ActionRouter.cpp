#include "CogCommonUI_ActionRouter.h"

#include "CogImguiInputHelper.h"
#include "CogSubsystem.h"

ERouteUIInputResult UCogCommonUI_ActionRouter::ProcessInput(FKey Key, EInputEvent InputEvent) const
{
	if (const UWorld* World = GetWorld())
	{
		if (const UPlayerInput* PlayerInput = FCogImguiInputHelper::GetPlayerInput(*World))
		{
			if (FCogImguiInputHelper::IsTopPriorityKey(*PlayerInput, Key))
			{
				return ERouteUIInputResult::Unhandled;
			}
		}
	}
	
	return UCommonUIActionRouterBase::ProcessInput(Key, InputEvent);
}

void UCogCommonUI_ActionRouter::PlayerControllerChanged(APlayerController* NewPlayerController)
{
	Super::PlayerControllerChanged(NewPlayerController);

	if (IsValid(NewPlayerController))
	{
		if (UWorld* World = NewPlayerController->GetWorld(); IsValid(World))
		{
			FCogImguiContext& CogImguiContext = World->GetSubsystem<UCogSubsystem>()->GetContext();
			if (CogInputChangeBinds.Contains(NewPlayerController))
			{
				CogImguiContext.CogInputStateChanged.Remove(CogInputChangeBinds[NewPlayerController]);
				CogInputChangeBinds.Remove(NewPlayerController);
			}

			CogInputChangeBinds.Add(NewPlayerController,
				CogImguiContext.CogInputStateChanged.AddUObject(this, &ThisClass::CogInputStateChanged));
		}
	}
}

void UCogCommonUI_ActionRouter::CogInputStateChanged(bool bNewState)
{
	// if we're disables, cede control back to CommonUI
	if (!bNewState)
	{
		RefreshUIInputConfig();
		RefreshActiveRootFocus();
	}
}
