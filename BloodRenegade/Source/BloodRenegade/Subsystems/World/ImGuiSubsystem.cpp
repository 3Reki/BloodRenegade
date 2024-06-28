#include "ImGuiSubsystem.h"

#include "ImGuiModule.h"
#include "BloodRenegade/Controller/BloodHUD.h"
#include "Kismet/GameplayStatics.h"

#if !UE_BUILD_SHIPPING

#include "GameFramework/CharacterMovementComponent.h"

#include "BloodRenegade/Controller/BloodController.h"
#include "BloodRenegade/Controller/BloodCharacter.h"
#include "BloodRenegade/Controller/BloodPlayerState.h"
#include "BloodRenegade/Controller/BloodCharacterDataAsset.h"

#include "BloodRenegade/Tools/BloodCheatManager.h"

#include <format>
#include <string>

#include "AIManagerSubsystem.h"
#include "imgui.h"
#include "imgui_internal.h"

namespace ImGui
{
	// No Unreal naming conventions inside this namespace, try to stick with ImGui ones 
	
	enum ImGuiMenuFlag : unsigned int
	{
		ImGuiMenuFlag_None					= 0,
		ImGuiMenuFlag_MainMenuBar			= 1,
		ImGuiMenuFlag_DemoWindow			= 2,
		ImGuiMenuFlag_MainCharacter			= 4,

		// put new enum values above, value must be 2^x
		ImGuiMenuFlag_Invalid				= 8
	};

	typedef unsigned int ImGuiMenuFlags;

	ImGuiMenuFlags ActiveMenus = ImGuiMenuFlag_None;

	bool BeginFlags(const char* name, const ImGuiMenuFlags menuFlags, const ImGuiWindowFlags flags = 0)
	{
		check(menuFlags > ImGuiMenuFlag_None && menuFlags < ImGuiMenuFlag_Invalid)
		
		bool open = true;
		const bool res = Begin(name, &open, flags);
		
		ActiveMenus ^= menuFlags * !open;

		return res;
	}
}

namespace ImGui {

	typedef int ImGuiKeyChord;

	void GetKeyChordName(ImGuiKeyChord key_chord, char* buffer, int buff_size)
	{
		ImFormatString(buffer, buff_size, "%s%s%s%s%s",
			(key_chord & ImGuiKey_ModCtrl) ? "Ctrl+" : "",
			(key_chord & ImGuiKey_ModShift) ? "Shift+" : "",
			(key_chord & ImGuiKey_ModAlt) ? "Alt+" : "",
			(key_chord & ImGuiKey_ModSuper) ? (GetIO().ConfigMacOSXBehaviors ? "Cmd+" : "Super+") : "",
			GetKeyName((ImGuiKey)(key_chord & ~ImGuiKey_ModMask)));
	}
	
	// This is the only public API until we expose owner_id versions of the API as replacements.
	bool IsKeyChordPressed(const ImGuiKeyChord key_chord)
	{
		const ImGuiKey mods = key_chord & ImGuiKey_ModMask;
		
		if ((GetIO().KeyMods << 11) != mods)
			return false;

		// Special storage location for mods
		const ImGuiKey key = key_chord & ~ImGuiKey_ModMask;
		if (key == ImGuiKey_None)
			return false;
		return IsKeyPressed(key);
	}
	
	// Wrapper for menu that can be opened with a global shortcut
	// or submenu with a local shortcut
	// inline bool BeginMenu(const char* label, const ImGuiKeyChord key)
	// {
	// 	if (ImGui::IsKeyChordPressed(key))
	// 		ImGui::OpenPopup(label);
	// 	return ImGui::BeginMenu(label);
	// };
	
	// Wrapper for menuitem that can be opened with a local shortcut
	bool MenuItem(const char* label, const ImGuiKeyChord key, bool selected = false, bool enabled = true)
	{
		char shortcut[32];
		GetKeyChordName(key, shortcut, IM_ARRAYSIZE(shortcut));
		return MenuItem(label, shortcut, selected, enabled) || IsKeyChordPressed(key);
	}
}


using namespace ImGui;

ABloodController* UImGuiSubsystem::GetPlayerController()
{
	if (!PlayerController.IsValid())
	{
		const UWorld* World = GetWorld();
		check(World);
		PlayerController = World->GetFirstPlayerController<ABloodController>();
	}
	
	return PlayerController.Get();
}

ABloodCharacter* UImGuiSubsystem::GetPlayerCharacter()
{
	const ABloodController* CurrentPlayerController = GetPlayerController();

	ABloodCharacter* TempController;
	
	if (CurrentPlayerController && IsValid(TempController = CurrentPlayerController->GetPawn<ABloodCharacter>()))
	{
		PlayerCharacter = TempController;
		return TempController;
	}

	TempController = PlayerCharacter.Get();
	if (!TempController)
	{
		TempController = Cast<ABloodCharacter>(UGameplayStatics::GetActorOfClass(GetWorld(), ABloodCharacter::StaticClass()));
		PlayerCharacter = TempController;
	}
	
	return TempController;
}

void UImGuiSubsystem::MainCharacterMenu()
{
	const ABloodController* CurrentPlayerController = GetPlayerController();
	check(CurrentPlayerController);

	UBloodCheatManager* CurrentCheatManager = Cast<UBloodCheatManager>(CurrentPlayerController->CheatManager);
	check(CurrentCheatManager);

	ABloodHUD* CurrentHUD = CurrentPlayerController->GetHUD<ABloodHUD>();
	check(CurrentCheatManager);

	if (MenuItem("Invincible", ImGuiKey_G, !CurrentPlayerController->GetPawn()->CanBeDamaged()))
		CurrentCheatManager->God();
	if (MenuItem("No Clip", ImGuiKey_N, CurrentPlayerController->GetCharacter()->GetCharacterMovement()->bCheatFlying))
		CurrentCheatManager->Ghost();
	if (MenuItem("Nightmare Form", ImGuiKey_F, CurrentPlayerController->CurrentPlayerState->IsInNightmareForm))
		CurrentCheatManager->ToggleNightmareForm();
	if (MenuItem("HUD Visibility", ImGuiKey_H, CurrentHUD->IsVisible))
		CurrentCheatManager->ToggleHUD();
	if (MenuItem("Main Character Settings", ImGuiKey_C, ActiveMenus & ImGuiMenuFlag_MainCharacter))
		ActiveMenus ^= ImGuiMenuFlag_MainCharacter;
	// if (ImGui::MenuItem("Redo", "CTRL+Y", false, false)) {}  // Disabled item
	// ImGui::Separator();
}

void UImGuiSubsystem::SoldierMenu()
{
	UAIManagerSubsystem* AIManager = GetWorld()->GetSubsystem<UAIManagerSubsystem>();
	
	if (MenuItem("Reset Soldiers", ImGuiKey_R))
		AIManager->ResetAllSoldiers();

	if (MenuItem("Detection Indicators Visibility", ImGuiKey_ModCtrl | ImGuiKey_H, AIManager->IsDetectionIndicatorVisible))
		AIManager->SetDetectionIndicatorVisible(!AIManager->IsDetectionIndicatorVisible);
}

void UImGuiSubsystem::MainMenuBar()
{
	if (BeginMainMenuBar())
	{
		//ProcessShortcuts();
		if (BeginMenu("Main Character", GetPlayerController() != nullptr))
		{
			MainCharacterMenu();
			ImGui::EndMenu(); // Don't remove ImGui:: to prevent compile error
		}

		if (BeginMenu("Soldiers"))
		{
			SoldierMenu();
			ImGui::EndMenu(); // Don't remove ImGui:: to prevent compile error
		}

		if (MenuItem("Show Demo Window"))
			ActiveMenus ^= ImGuiMenuFlag_DemoWindow;

		{
			FString AppVersion;
			GConfig->GetString(
				TEXT("/Script/EngineSettings.GeneralProjectSettings"),
				TEXT("ProjectVersion"),
				AppVersion,
				GGameIni
			);

			const char* AppVersionText = TCHAR_TO_ANSI(*AppVersion);
			
			const std::string FPSString = std::format("{:.0f} FPS ({:.2f} ms)", GetIO().Framerate, 1000.0f / GetIO().Framerate);
			const char* FPSText = FPSString.c_str();
			SameLine(GetWindowWidth() - CalcTextSize(FPSText).x - CalcTextSize(AppVersionText).x - 120);
			TextUnformatted(FPSText);
			TextUnformatted("-- Version :");
			TextUnformatted(AppVersionText);
		}

		EndMainMenuBar();
	}
}

void UImGuiSubsystem::MainCharacterWindow()
{
	if (BeginFlags("Main Character", ImGuiMenuFlag_MainCharacter))
	{
		const ABloodController* CurrentPlayerController = GetPlayerController();
		check(CurrentPlayerController);

		ABloodCharacter* CurrentPlayerCharacter = GetPlayerCharacter();
		check(CurrentPlayerCharacter);

		UBloodCheatManager* CheatManager = Cast<UBloodCheatManager>(CurrentPlayerController->CheatManager);

		bool b;
		if (Checkbox("Invincible", &(b = !CurrentPlayerController->GetPawn()->CanBeDamaged())))
			CheatManager->God();
		
		if (Checkbox("No Clip", &(b = CurrentPlayerCharacter->GetCharacterMovement()->bCheatFlying)))
			CheatManager->Ghost();

		Checkbox("Cooldowns Enabled", &CurrentPlayerCharacter->CooldownEnable);
		Checkbox("Blood Gauge Enabled", &CurrentPlayerCharacter->BloodGaugeEnable);
		

		int CurrentHP = CurrentPlayerCharacter->CurrentPlayerState->CurrentHP;
		int MaxHP = CurrentPlayerCharacter->DataAsset->MaxHp;
		if (SliderInt("Current HP", &CurrentHP, 1, MaxHP, "%d", ImGuiSliderFlags_AlwaysClamp))
			CheatManager->SetHP(CurrentHP);
		

		const char* NightmareButtonName = CurrentPlayerCharacter->CurrentPlayerState->IsInNightmareForm
			? "Disable Nightmare Form"
			: "Enable Nightmare Form";
		if (Button(NightmareButtonName))
			CheatManager->ToggleNightmareForm();

		float MaxGauge = CurrentPlayerCharacter->DataAsset->MaxBloodGauge;
		//if (SliderInt("Blood Gauge", &CurrentPlayerCharacter->CurrentPlayerState->CurrentBloodGauge, 0, MaxGauge, "%.0f", ImGuiSliderFlags_AlwaysClamp))
		//	CurrentPlayerController->CurrentHUD->SetPercentBloodGauge(CurrentPlayerCharacter->CurrentPlayerState->CurrentBloodGauge);
	}

	End();
}

void UImGuiSubsystem::SetMainMenuActive(const bool State)
{
	ActiveMenus &= ~ImGuiMenuFlag_MainMenuBar;
	if (State)
	{
		ActiveMenus |= State * ImGuiMenuFlag_MainMenuBar;
	}
	else
	{
		GetPlayerController()->SetInputMode(FInputModeGameOnly());
	}
}

void UImGuiSubsystem::BeginDestroy()
{
	Super::BeginDestroy();

	FImGuiModuleProperties ImGuiModuleProperties = FImGuiModule::Get().GetProperties();
	ImGuiModuleProperties.SetInputEnabled(false);
	ImGuiModuleProperties.OnSetInput.Remove(OnImguiSetInputHandle);
}

void UImGuiSubsystem::OnWorldBeginPlay(UWorld& InWorld)
{
	Super::OnWorldBeginPlay(InWorld);

	OnImguiSetInputHandle = FImGuiModule::Get().GetProperties().OnSetInput.AddUObject(this, &UImGuiSubsystem::SetMainMenuActive);
}

void UImGuiSubsystem::Tick(const float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (ActiveMenus & ImGuiMenuFlag_MainMenuBar) MainMenuBar();
	if (ActiveMenus & ImGuiMenuFlag_DemoWindow) ShowDemoWindow(); 
	if (ActiveMenus & ImGuiMenuFlag_MainCharacter) MainCharacterWindow();
}

#endif
