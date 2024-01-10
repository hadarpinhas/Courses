c4-week2-Exercise7- Launchers
c2 - Visual - Studio - Programming - Assignment
1. ProjectileActor.cpp
#include "ProjectileActor.h"
/**
 * Sets default values
*/
AProjectileActor::AProjectileActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}
void AProjectileActor::BeginPlay()
{
	Super::BeginPlay();
	// save bounds for actor
	FVector Origin;
	FVector BoxExtent;
	GetActorBounds(true, Origin, BoxExtent);
	HalfCollisionWidth = BoxExtent.Y;
	TArray<UStaticMeshComponent*> StaticMeshComponents;
	GetComponents(StaticMeshComponents);
	// make sure we found the static mesh
	if (StaticMeshComponents.Num() > 0)
	{
		// the mesh we want is at location 0 because there's only 1 mesh
		UStaticMeshComponent* StaticMesh = StaticMeshComponents[0];

		// add impulse force to mesh
		StaticMesh->AddImpulse(ImpulseForce);
	}
}
void AProjectileActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (GetActorLocation().Y + HalfCollisionWidth >
		ScreenRight)
	{
		Destroy();
	}
}
void AProjectileActor::SetImpulseForce(FVector Force)
{
	ImpulseForce = Force;
}

ProjectileActor.h
#pragma once

#include "CoreMinimal.h"
#include "ProjectileActor.h"
#include "PirateProjectileActor.generated.h"

/**
 * A pirate projectile
 */
UCLASS()
class LAUNCHERS_API APirateProjectileActor : public AProjectileActor
{
	GENERATED_BODY()

public:
	/**
	 * Sets default values for this actor's properties
	*/
	APirateProjectileActor();
};

2. LauncherActor.cpp

#include "LauncherActor.h"
/**
 * Sets default values
*/
ALauncherActor::ALauncherActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
}
void ALauncherActor::BeginPlay()
{
	Super::BeginPlay();

	// save bounds for actor
	FVector Origin;
	FVector BoxExtent;
	GetActorBounds(true, Origin, BoxExtent);
	HalfCollisionWidth = BoxExtent.Y;
	StartCooldownTimer();
}

void ALauncherActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
void ALauncherActor::SetCooldownSeconds(float Seconds)
{
	if (Seconds > 0)
	{
		CooldownSeconds = Seconds;
	}
}
void ALauncherActor::SpawnProjectile()
{
	// spawn projectile
	FVector SpawnLocation{ GetActorLocation() };
	SpawnLocation.Y += HalfCollisionWidth + ProjectileSpawnOffset;
	GetWorld()->SpawnActor<AProjectileActor>(UProjectile, SpawnLocation,FRotator::ZeroRotator);
	StartCooldownTimer();
}
void ALauncherActor::StartCooldownTimer()
{
	FTimerHandle Timer;
	GetWorldTimerManager().SetTimer(Timer, this,
		&ALauncherActor::SpawnProjectile,
		CooldownSeconds);
}

LauncherActor.h
// Copyright A.T. Chamillard. All Rights Reserved.
#pragma once
#include "ProjectileActor.h"
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LauncherActor.generated.h"

UCLASS()
class LAUNCHERS_API ALauncherActor : public AActor
{
	GENERATED_BODY()

private:
	float CooldownSeconds{ 0 };
	float HalfCollisionWidth;
	float ProjectileSpawnOffset{ 15 };
	void StartCooldownTimer();

public:
	UPROPERTY(EditAnywhere,meta = (MetaClass = "Projectile"),Category = ProjectileBlueprints)  TSubclassOf<AProjectileActor> UProjectile;

	*/
	UFUNCTION()
		void SpawnProjectile();

public:
	ALauncherActor();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

protected:
	void SetCooldownSeconds(float Seconds);
};

3. ChainsawLauncherActor.cpp
#include "ChainsawLauncherActor.h"
AChainsawLauncherActor::AChainsawLauncherActor() :
	ALauncherActor{}
{
	SetCooldownSeconds(2.0f);
}

ChainsawLauncherActor.h
#pragma once
#include "CoreMinimal.h"
#include "LauncherActor.h"
#include "ChainsawLauncherActor.generated.h"
UCLASS()
class LAUNCHERS_API AChainsawLauncherActor : public ALauncherActor
{
	GENERATED_BODY()

public:
	AChainsawLauncherActor();
};

4. LauncherSpawnerActor.cpp
#include "LauncherSpawnerActor.h"
#include "Kismet/GameplayStatics.h"
ALauncherSpawnerActor::ALauncherSpawnerActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}
void ALauncherSpawnerActor::BeginPlay()
{
	Super::BeginPlay();

	StartSpawnTimer();
}
void ALauncherSpawnerActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
void ALauncherSpawnerActor::SpawnLauncher()
{
	// try to find Actor with tag
	TArray<AActor*> TaggedActors;
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), "Launcher", TaggedActors);
	// destroy first Actor in TArray
	if (TaggedActors.Num() > 0)
	{
		TaggedActors[0]->Destroy();
	}

	// randomly pick which launcher to spawn
	TSubclassOf<ALauncherActor> SpawnLauncher;
	int LauncherNumber = FMath::RandRange(0, 2);
	if (LauncherNumber == 0)
	{
		SpawnLauncher = UChainsawLauncher;
	}
	else if (LauncherNumber == 1)
	{
		SpawnLauncher = UPirateLauncher;
	}
	else
	{
		SpawnLauncher = UZombieLauncher;
	}
	// spawn the launcher
	GetWorld()->SpawnActor<ALauncherActor>(SpawnLauncher, FVector::ZeroVector,	FRotator::ZeroRotator);
	// start the timer again
	StartSpawnTimer();
}
void ALauncherSpawnerActor::StartSpawnTimer()
{
	// start the timer
	FTimerHandle Timer;
	GetWorldTimerManager().SetTimer(Timer, this,&ALauncherSpawnerActor::SpawnLauncher,SpawnDelaySeconds);
}

LauncherSpawnerActor.h
#pragma once
#include "LauncherActor.h"
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LauncherSpawnerActor.generated.h"
UCLASS()
class LAUNCHERS_API ALauncherSpawnerActor : public AActor
{
	GENERATED_BODY()
private:
	const float SpawnDelaySeconds{ 5 };

	void StartSpawnTimer();
public:
	UPROPERTY(EditAnywhere,	meta = (MetaClass = "Launcher"),Category = LauncherBlueprints) TSubclassOf<ALauncherActor> UChainsawLauncher;
	UPROPERTY(EditAnywhere,	meta = (MetaClass = "Launcher"),Category = LauncherBlueprints) TSubclassOf<ALauncherActor> UPirateLauncher;
	UPROPERTY(EditAnywhere,	meta = (MetaClass = "Launcher"),Category = LauncherBlueprints)	TSubclassOf<ALauncherActor> UZombieLauncher;
	UFUNCTION()
		void SpawnLauncher();
	ALauncherSpawnerActor();
protected:
	virtual void BeginPlay() override;
public:
	virtual void Tick(float DeltaTime) override;
};


5. set tag "Launcher"
6. add to the blupeprint the U property Launcher Bluerpint





c4-week2-exercise6- ATeddyBearSpawner
1. create a c++ class Actor named BallSpawnerActor:
2. copy the code to .h:
#pragma once
#include "BallActor.h"
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BallSpawnerActor.generated.h"
/**
 * A ball spawner
*/
UCLASS()
class MOVEIT_API ABallSpawnerActor : public AActor
{
	GENERATED_BODY()

private:
	// spawn delay support
	const float MinSpawnDelaySeconds{ 1 };
	const float MaxSpawnDelaySeconds{ 2 };
	// spawn location support
	const float MinSpawnY{ -200 };
	const float MaxSpawnY{ 200 };
	const float MinSpawnZ{ 0 };
	const float MaxSpawnZ{ 100 };
	/**
	 * Starts the spawn timer
	*/
	void StartSpawnTimer();
public:
	UPROPERTY(EditAnywhere,meta = (MetaClass = "Ball"),Category = BallBlueprints)
		TSubclassOf<ABallActor> UGreenBall;
	UPROPERTY(EditAnywhere,meta = (MetaClass = "Ball"),Category = BallBlueprints)
		TSubclassOf<ABallActor> URedBall;
	UPROPERTY(EditAnywhere,meta = (MetaClass = "Ball"),	Category = BallBlueprints)
		TSubclassOf<ABallActor> UWhiteBall;
	/**
	 * Spawns a ball
	*/
	UFUNCTION()
		void SpawnBall();
	/**
	 * Sets default values for this actor's properties
	*/
	ABallSpawnerActor();
protected:
	/**
	 * Called when the game starts or when spawned
	*/
	virtual void BeginPlay() override;
public:
	/**
	 * Called every frame
	 * @param DeltaTime Game time elapsed during last frame modified by the time dilation
	*/
	virtual void Tick(float DeltaTime) override;

};

3. copy the code to .cpp:
#include "BallSpawnerActor.h"
/**
 * Sets default values
*/
ABallSpawnerActor::ABallSpawnerActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
}

/**
 * Called when the game starts or when spawned
*/
void ABallSpawnerActor::BeginPlay()
{
	Super::BeginPlay();
	StartSpawnTimer();
}
/**
 * Called every frame
 * @param DeltaTime Game time elapsed during last frame modified by the time dilation
*/
void ABallSpawnerActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
/**
 * Spawns a ball
*/
void ABallSpawnerActor::SpawnBall()
{
	// randomly pick which ball to spawn
	TSubclassOf<ABallActor> SpawnBall;
	int TeddyBearNumber = FMath::RandRange(0, 2);
	if (TeddyBearNumber == 0)
	{
		SpawnBall = UGreenBall;
	}
	else if (TeddyBearNumber == 1)
	{
		SpawnBall = URedBall;
	}
	else
	{
		SpawnBall = UWhiteBall;
	}
	// generate random spawn location
	FVector SpawnLocation{ 0 };
	SpawnLocation.Y = FMath::RandRange(MinSpawnY, MaxSpawnY);
	SpawnLocation.Z = FMath::RandRange(MinSpawnZ, MaxSpawnZ);

	// spawn the teddy bear
	GetWorld()->SpawnActor<ABallActor>(SpawnBall, SpawnLocation,FRotator::ZeroRotator);
	// start the timer again
	StartSpawnTimer();
}
/**
 * Starts the spawn timer
*/
void ABallSpawnerActor::StartSpawnTimer()
{
	// set random duration and start the timer
	float TimerDuration = FMath::RandRange(MinSpawnDelaySeconds, MaxSpawnDelaySeconds);
	FTimerHandle Timer;
	GetWorldTimerManager().SetTimer(Timer, this,&ABallSpawnerActor::SpawnBall,TimerDuration);
}






//					configuration data table
1. add ConfigurationDataStruct.h to source files of the project and build even with the errors shown

							#pragma once 
							#include "Engine/DataTable.h"
							#include "ConfigurationDataStruct.generated.h"
							USTRUCT(BlueprintType)
							struct FConfigurationDataStruct : public FTableRowBase
							{
								GENERATED_USTRUCT_BODY()
							public:
								FConfigurationDataStruct() : TeddyBearMoveAmountPerSecond(100), ConfigData2(200) {}
								UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Configuration Data Struct")
									float TeddyBearMoveAmountPerSecond;
								UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Configuration Data Struct")
									float ConfigData2;
							};

2. import csv file (created first located next to .sln) to UE content/ Exercise 4 / Data, choose DataTableRowType: ConfigurationDataStruct

3. create a an actor class ConfigurationDataActor in   c++ Classes UnrealDataTable / Configuration.h , copy this code and build
										#pragma once
										#include "Engine/DataTable.h"
										#include "ConfigurationDataStruct.h"
										#include "CoreMinimal.h"
										#include "GameFramework/Actor.h"
										#include "ConfigurationDataActor.generated.h"
										UCLASS()
										class UNREALDATATABLE_API AConfigurationDataActor : public AActor
										{
											GENERATED_BODY()
										private:
											// saved for efficiency
											FConfigurationDataStruct* ConfigurationDataRow;
										public:
											UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Configuration Data Table")
												UDataTable* ConfigurationDataTable;
											AConfigurationDataActor();

										protected:
											virtual void BeginPlay() override;
										public:
											virtual void Tick(float DeltaTime) override;
											float GetTeddyBearMoveAmountPerSecond();
										};

											#include "ConfigurationDataActor.h"
											AConfigurationDataActor::AConfigurationDataActor()
											{
												// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
												PrimaryActorTick.bCanEverTick = false;
											}

											void AConfigurationDataActor::BeginPlay()
											{
												Super::BeginPlay();

												// retrieve configuration data from data table
												FString ContextString;
												ConfigurationDataRow = ConfigurationDataTable->FindRow<FConfigurationDataStruct>("ConfigData", ContextString);
											}

											void AConfigurationDataActor::Tick(float DeltaTime)
											{
												Super::Tick(DeltaTime);
											}

											float AConfigurationDataActor::GetTeddyBearMoveAmountPerSecond()
											{
												return ConfigurationDataRow->TeddyBearMoveAmountPerSecond;
											}

4. turn the actor into blueprint and add the ConfigurationData (Data table) we imported compile and save
5. pull the ConfigurationDataActor BP to the game

6. Tag: edit - > ProjectSettings - > search for gamePlay tag, add gameplay tag to list, Name:ConfigurationDataActor
7. open BP_COnfigurationDataActor, search for tags and add exactly ConfigurationDataActor compile and save
8. In the c+++ code TeddyBearPawn.h add #include "ConfigurationDataActor.h" and change //const float MoveAmountPerSecond{ 100 };
and add 			AConfigurationDataActor * ConfigurationData;
		private:
			//const float MoveAmountPerSecond{ 100 };
			float HalfCollisionWidth;
			float HalfCollisionHeight;

			// saved for efficiency
			AConfigurationDataActor* ConfigurationData;
9. add to TeddyBearPawn.cpp: 
#include "Kismet/GameplayStatics.h"
and in beginPlay add:
// save configuration data for efficiency
TArray<AActor*> ConfigurationDataActors;
UGameplayStatics::GetAllActorsWithTag(GetWorld(), "ConfigurationDataActor", ConfigurationDataActors);
if (ConfigurationDataActors.Num() > 0)
{
	ConfigurationData = (AConfigurationDataActor*)ConfigurationDataActors[0];
}

and update 
NewLocation.Y += PendingMovementInput.Y * MoveAmountPerSecond() * DeltaTime; to
NewLocation.Y += PendingMovementInput.Y * ConfigurationData->GetTeddyBearMoveAmountPerSecond() * DeltaTime;

In total TeddyBearPawn.cpp:
// Copyright A.T. Chamillard. All Rights Reserved.
#include "TeddyBearPawn.h"
#include "Kismet/GameplayStatics.h"
#include "ScreenConstants.h"
/**
 * Sets default values
*/
ATeddyBearPawn::ATeddyBearPawn()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}
/**
 * Called when the game starts or when spawned
*/
void ATeddyBearPawn::BeginPlay()
{
	Super::BeginPlay();

	// save bounds for pawn
	FVector Origin;
	FVector BoxExtent;
	GetActorBounds(true, Origin, BoxExtent);
	HalfCollisionWidth = BoxExtent.Y;
	HalfCollisionHeight = BoxExtent.Z;
	// save configuration data for efficiency
	TArray<AActor*> ConfigurationDataActors;
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), "ConfigurationDataActor", ConfigurationDataActors);
	if (ConfigurationDataActors.Num() > 0)
	{
		ConfigurationData = (AConfigurationDataActor*)ConfigurationDataActors[0];
	}
}

/**
 * Called every frame
 * @param DeltaTime Game time elapsed during last frame modified by the time dilation
*/
void ATeddyBearPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	// move pawn based on movement inputs
	FVector PendingMovementInput = ConsumeMovementInputVector();
	if (PendingMovementInput.Y != 0 ||
		PendingMovementInput.Z != 0)
	{
		FVector NewLocation = GetActorLocation();
		NewLocation.Y += PendingMovementInput.Y * ConfigurationData->GetTeddyBearMoveAmountPerSecond() * DeltaTime;
		NewLocation.Y = FMath::Clamp(NewLocation.Y, ScreenConstants::Left + HalfCollisionWidth, ScreenConstants::Right - HalfCollisionWidth);
		NewLocation.Z += PendingMovementInput.Z * ConfigurationData->GetTeddyBearMoveAmountPerSecond() * DeltaTime;
		NewLocation.Z = FMath::Clamp(NewLocation.Z, ScreenConstants::Bottom + HalfCollisionHeight, ScreenConstants::Top - HalfCollisionHeight);
		SetActorLocation(NewLocation);
	}
}
/**
 * Moves the teddy bear horizontally
 * @param moveScale scale factor for movement
*/
void ATeddyBearPawn::MoveHorizontally(float moveScale)
{
	moveScale = FMath::Clamp(moveScale, -1.0f, 1.0f);
	AddMovementInput(GetActorRightVector(),
		moveScale);
}

/**
 * Moves the teddy bear vertically
 * @param moveScale scale factor for movement
*/
void ATeddyBearPawn::MoveVertically(float moveScale)
{
	moveScale = FMath::Clamp(moveScale, -1.0f, 1.0f);
	AddMovementInput(GetActorUpVector(),
		moveScale);
}




//				Move pawn to end of screen and kill it
1. camera -500,0,0, 0,0,0, static, orthographic - Blueprins - > 1. get player controller pull to open 2. set view with blend. 3 camer actor. 4. event begin play draw to set view.
2. light source -500,0,100,  0,-10,10 station
3. edit -> project settings -> <the game mode>
3. Map0 inside Maps
4. "Class'/Game/Exercise8/Blueprints/BP_MyPawn.BP_MyPawn_C'" in gamemode.cpp   make sure to create folder in content and only then: Art, Blueprints, Maps
								Ac3_Exercise8GameModeBase::Ac3_Exercise8GameModeBase()
								{
									// set pawn and player controller
									ConstructorHelpers::FObjectFinder<UClass>
										PawnClass(TEXT("Class'/Game/Exercise8/Blueprints/BP_MyPawn.BP_MyPawn_C'"));
									if (PawnClass.Object != nullptr)
									{
										DefaultPawnClass = PawnClass.Object;
									}
5. binding (after edit->project setings->input):
								void AMyPlayerController::SetupInputComponent()
								{
									Super::SetupInputComponent();

									// set up movement key binding
									check(InputComponent != nullptr);
									InputComponent->BindAxis("Horizontal", this, &AMyPlayerController::MoveHorizontally);
6. in Pawn.cpp: each tick SetActorLocation(NewLocation);

Add in GameHUD.h  //////// HUD ////////////				printing
public: 
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = HUDFont) 
	UFont* HudFont;
	virtual void DrawHUD() override;
after compile VS
HUD - create GameHUD class from UE, create BP_GameModeBase from GameModeBase class in UE and save, edit->project settings -> <BP_the game mode>
Blueprint menu in top screen -> GameMode: edit... (if not appear choose GameMode: -> select->BP_GameModeBase)-> HUD:HUD -> create -> GameHUD
open BP_GameHUD dropdown on None -> Font, save in TheRunaway as GameHudFont, compile and save
go to TeRunaway folder and open GameHudFont -> Add Font -> "choose a font file from this computer" (folder icon)->"C:\Windows\Font_hadar" and save
add this code to HunterPawn:
#include "Kismet/GameplayStatics.h"
#include "GameHUD.h"
SetActorLocation(FVector::ZeroVector);
Health -= 25;
AGameHUD* Hud = UGameplayStatics::GetPlayerController(this, 0)->GetHUD<AGameHUD>();
if (Hud != nullptr) {
	Hud->addDamage();
}
if (Health <= 0)
{
	Destroy();
}

Add in UE  //////// Sound ////////////
new Folder Sounds, inside folder right click-> import the wav sound
right click->sounds ->soundcue, double click sound cue to open BP. right click and open Wave Player and connect them. choose in WavePlayer dropdown instead of None, the correct sound Cue and save.
in Pawn.h: #include "Sound/SoundCue.h", make sure #include "HunterPawn.generated.h" is last
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
		USoundCue* AudioCue;
	
in Pawn.cpp:
UGameplayStatics::PlaySound2D(this, AudioCue);
Now, in the UE open BP_HunterPawn editor, and there will be Audio Cue->choose correct Audio, compile and save.

// Copyright A.T. Chamillard. All Rights Reserved.


#include "TeddyBear.h"

/**
 * Sets default values
*/
ATeddyBear::ATeddyBear()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}

/**
 * Called when the game starts or when spawned
*/
void ATeddyBear::BeginPlay()
{
	Super::BeginPlay();
	
}

/**
 * Called every frame
 * @param DeltaTime Game time elapsed during last frame modified by the time dilation
*/
void ATeddyBear::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}




// Copyright A.T. Chamillard. All Rights Reserved.


#include "MyPawn.h"

/**
 * Sets default values
*/
AMyPawn::AMyPawn()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

/**
 * Called when the game starts or when spawned
*/
void AMyPawn::BeginPlay()
{
	Super::BeginPlay();

}

/**
 * Called every frame
 * @param DeltaTime Game time elapsed during last frame modified by the time dilation
*/
void AMyPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// move pawn based on movement inputs
	FVector PendingMovementInput = ConsumeMovementInputVector();
	if (PendingMovementInput.Y != 0 || PendingMovementInput.Z != 0) {

		FVector NewLocation = GetActorLocation();

		PendingMovementInput.Y = FMath::Clamp(PendingMovementInput.Y, -1.0f, 1.0f);
		NewLocation.Y += PendingMovementInput.Y;

		PendingMovementInput.Z = FMath::Clamp(PendingMovementInput.Z, -1.0f, 1.0f);
		NewLocation.Z += PendingMovementInput.Z;

		SetActorLocation(NewLocation);
	}
}

/**
 * Moves the pawn horizontally
 * @param moveScale scale factor for movement
*/
void AMyPawn::MoveHorizontally(float moveScale)
{
	AddMovementInput(GetActorRightVector(), moveScale * MoveAmountPerFrame);
}

/**
 * Moves the pawn vertically
 * @param moveScale scale factor for movement
*/
void AMyPawn::MoveVertically(float moveScale)
{
	AddMovementInput(GetActorUpVector(), moveScale * MoveAmountPerFrame);
}




// Copyright A.T. Chamillard. All Rights Reserved.


#include "MyPlayerController.h"

#include "MyPawn.h"

/**
 * Called to bind functionality to input
*/
void AMyPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	// set up movement key bindings
	check(InputComponent != nullptr);

	InputComponent->BindAxis("Horizontal", this, &AMyPlayerController::MoveHorizontally);

	InputComponent->BindAxis("Vertical", this, &AMyPlayerController::MoveVertically);

}

/**
 * Moves the pawn horizontally
 * @param input input on Horizontal axis
*/
void AMyPlayerController::MoveHorizontally(float input)
{
	// tell pawn to move horizontally as appropriate
	if (input != 0) {
		AMyPawn* MyPawn = (AMyPawn*)GetPawn();
		if (MyPawn != nullptr) {
			MyPawn->MoveHorizontally(input);
		}
	}
}

/**
 * Moves the pawn vertically
 * @param input input on Vertical axis
*/
void AMyPlayerController::MoveVertically(float input)
{
	// tell pawn to move vertically as appropriate
	if (input != 0) {
		AMyPawn* MyPawn = (AMyPawn*)GetPawn();
		if (MyPawn != nullptr) {
			MyPawn->MoveVertically(input);
		}
	}
}





// Copyright Epic Games, Inc. All Rights Reserved.


#include "FinallyADrivingGameGameModeBase.h"

/**
 * Sets default values
*/
AFinallyADrivingGameGameModeBase::AFinallyADrivingGameGameModeBase()
{

	// set pawn and player controller
	ConstructorHelpers::FObjectFinder<UClass>
		PawnClass(TEXT("class'/Game/FinallyADrivingGame/Blueprints/BP_MyPawn.BP_MyPawn_C'"));
	if (PawnClass.Object != nullptr)
	{
		DefaultPawnClass = PawnClass.Object;
	}
	ConstructorHelpers::FObjectFinder<UClass>
		ControllerClass(TEXT("class'/Game/FinallyADrivingGame/Blueprints/BP_MyPlayerController.BP_MyPlayerController_C'"));
	if (ControllerClass.Object != nullptr)
	{
		PlayerControllerClass = ControllerClass.Object;
	}
	// How it is done in video, and worked in MouseJumping () c2Ex5
	//ConstructorHelpers::FObjectFinder<UBlueprint>Blueprint(TEXT("Blueprint'/Game/FinallyADrivingGame/Blueprints/BP_MyPawn.BP_MyPawn'"));
	//if (Blueprint.Object != nullptr)
	//{
	//	DefaultPawnClass = (UClass*)Blueprint.Object->GeneratedClass;
	//}
	//PlayerControllerClass = AFinallyADrivingGameGameModeBase::StaticClass();
}





// Copyright A.T. Chamillard. All Rights Reserved.


#include "MinerPlayerController.h"

#include "MinerPawn.h"

/**
 * Called to bind functionality to input
*/
void AMinerPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	// check input component
	check(InputComponent != nullptr);


	// bind the jump miner action, when the button is released,
	// to the JumpMinerPawn function
	InputComponent->BindAction("JumpMiner", EInputEvent::IE_Pressed, this, &AMinerPlayerController::JumpMinerPawn);
}

/**
 * Jumps the miner pawn
*/
void AMinerPlayerController::JumpMinerPawn()
{
	// convert mouse position to world location
	FVector WorldLocation;
	FVector WorldDirection;
	DeprojectMousePositionToWorld(WorldLocation, WorldDirection);

	// move location to y-z plane and move pawn
	WorldLocation.X = 0;
	AMinerPawn* MinerPawn = (AMinerPawn*)GetPawn();
	if (MinerPawn != nullptr) {
		MinerPawn->SetLocation(WorldLocation);
	}
}

// Copyright Epic Games, Inc. All Rights Reserved.


#include "MouseJumpingGameModeBase.h"

#include "MinerPlayerController.h"

/**
 * Sets default values
*/
AMouseJumpingGameModeBase::AMouseJumpingGameModeBase()
{
	// set pawn and player controller
	ConstructorHelpers::FObjectFinder<UBlueprint>
		Blueprint(TEXT("Blueprint'/Game/MouseJumping/Blueprints/BP_MinerPawn.BP_MinerPawn'"));
	if (Blueprint.Object != nullptr)
	{
		DefaultPawnClass = (UClass*)Blueprint.Object->GeneratedClass;
	}
	PlayerControllerClass = AMinerPlayerController::StaticClass();
}







// Copyright A.T. Chamillard. All Rights Reserved.


#include "Destroyer.h"

#include "TeddyBear.h"

/**
 * Sets default values
*/
ADestroyer::ADestroyer()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

/**
 * Called when the game starts or when spawned
*/
void ADestroyer::BeginPlay()
{
	Super::BeginPlay();

	StartDestroyTimer();
}

/**
 * Called every frame
 * @param DeltaTime Game time elapsed during last frame modified by the time dilation
*/
void ADestroyer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

/**
 * Adds the given teddy bear to the TArray
*/
void ADestroyer::AddTeddyBear(ATeddyBear* TeddyBear)
{
	// add code here to add the given
	// teddy bear to the TArray
	TeddyBears.Add(TeddyBear);
}

/**
 * Destroys a random teddy bear in the TArray
*/
void ADestroyer::DestroyTeddyBear()
{
	if (TeddyBears.Num() > 0)
	{
		// add code here to randomly pick a teddy 
		// bear index, save the teddy bear at that
		// index, remove the teddy bear from the
		// TArray, and destroy it
		int Index = FMath::RandRange(0, TeddyBears.Num() - 1);
		ATeddyBear* TeddyBear = TeddyBears[Index];
		TeddyBears.RemoveAt(Index);
		TeddyBear->Destroy();
	}

	// restart the destroy timer
	StartDestroyTimer();
}

/**
 * Starts the destroy timer
*/
void ADestroyer::StartDestroyTimer()
{
	// start 1 second timer
	FTimerHandle Timer;
	GetWorldTimerManager().SetTimer(Timer, this,
		&ADestroyer::DestroyTeddyBear,
		2.0f);
}





// Copyright A.T. Chamillard. All Rights Reserved.

#pragma once

#include "Destroyer.h"
#include "TeddyBear.h"

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TeddyBearSpawner.generated.h"

/**
 * The teddy bear spawner
*/
UCLASS()
class RANDOMDESTRUCTION_API ATeddyBearSpawner : public AActor
{
	GENERATED_BODY()

private:
	ADestroyer* Destroyer;

	// spawn location support
	const float MinSpawnY{ -200 };
	const float MaxSpawnY{ 200 };
	const float MinSpawnZ{ 0 };
	const float MaxSpawnZ{ 100 };

	/**
	 * Starts the spawn timer
	*/
	void StartSpawnTimer();

public:
	UPROPERTY(EditAnywhere,
		meta = (MetaClass = "TeddyBear"),
		Category = TeddyBearBlueprints)
		TSubclassOf<ATeddyBear> UGreenTeddyBear;
	UPROPERTY(EditAnywhere,
		meta = (MetaClass = "TeddyBear"),
		Category = TeddyBearBlueprints)
		TSubclassOf<ATeddyBear> UPurpleTeddyBear;
	UPROPERTY(EditAnywhere,
		meta = (MetaClass = "TeddyBear"),
		Category = TeddyBearBlueprints)
		TSubclassOf<ATeddyBear> UYellowTeddyBear;

	/**
	 * Spawns a teddy bear
	*/
	UFUNCTION()
		void SpawnTeddyBear();
public:
	/**
	 * Sets default values for this actor's properties
	*/
	ATeddyBearSpawner();

protected:
	/**
	 * Called when the game starts or when spawned
	*/
	virtual void BeginPlay() override;

public:
	/**
	 * Called every frame
	 * @param DeltaTime Game time elapsed during last frame modified by the time dilation
	*/
	virtual void Tick(float DeltaTime) override;

};





// random destruction c2 - Exercise - 13 - Sol

// Copyright A.T. Chamillard. All Rights Reserved.


#include "TeddyBearSpawner.h"

#include "Kismet/GameplayStatics.h"

/**
 * Sets default values
*/
ATeddyBearSpawner::ATeddyBearSpawner()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

/**
 * Called when the game starts or when spawned
*/
void ATeddyBearSpawner::BeginPlay()
{
	Super::BeginPlay();

	// save Destroyer for efficiency
	TArray<AActor*> TaggedActors;
	UGameplayStatics::GetAllActorsWithTag(
		GetWorld(), "Destroyer", TaggedActors);
	if (TaggedActors.Num() > 0)
	{
		Destroyer = (ADestroyer*)TaggedActors[0];
	}

	StartSpawnTimer();
}

/**
 * Called every frame
 * @param DeltaTime Game time elapsed during last frame modified by the time dilation
*/
void ATeddyBearSpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
/**
 * Spawns a teddy bear
*/
void ATeddyBearSpawner::SpawnTeddyBear()
{
	// randomly pick which teddy bear to spawn
	TSubclassOf<ATeddyBear> SpawnTeddyBear;
	int TeddyBearNumber = FMath::RandRange(0, 2);
	if (TeddyBearNumber == 0)
	{
		SpawnTeddyBear = UGreenTeddyBear;
	}
	else if (TeddyBearNumber == 1)
	{
		SpawnTeddyBear = UPurpleTeddyBear;
	}
	else
	{
		SpawnTeddyBear = UYellowTeddyBear;
	}

	// generate random spawn location
	FVector SpawnLocation{ 0 };
	SpawnLocation.SetComponentForAxis(EAxis::Y,
		FMath::RandRange(MinSpawnY, MaxSpawnY));
	SpawnLocation.SetComponentForAxis(EAxis::Z,
		FMath::RandRange(MinSpawnZ, MaxSpawnZ));

	// change code below to save the
	// spawned teddy bear and call the
	// Destroyer AddTeddyBear function
	// Note: The Destroyer field holds a
	// pointer to the Destroyer Actor
	/*GetWorld()->SpawnActor<ATeddyBear>(
		SpawnTeddyBear, SpawnLocation,
		FRotator::ZeroRotator);*/
	ATeddyBear* TeddyBear = GetWorld()->SpawnActor<ATeddyBear>(
		SpawnTeddyBear, SpawnLocation,
		FRotator::ZeroRotator);
	Destroyer->AddTeddyBear(TeddyBear);

	// start the timer again
	StartSpawnTimer();
}

/**
 * Starts the spawn timer
*/
void ATeddyBearSpawner::StartSpawnTimer()
{
	// start 1 second timer
	FTimerHandle Timer;
	GetWorldTimerManager().SetTimer(Timer, this,
		&ATeddyBearSpawner::SpawnTeddyBear,
		1.0f);
}





// c3 - Exercise - 2 - Solution

// Copyright A.T. Chamillard. All Rights Reserved.


#include "Newton.h"
#include "Apple.h"

/**
 * Sets default values
*/
ANewton::ANewton()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}

/**
 * Called when the game starts or when spawned
*/
void ANewton::BeginPlay()
{
	Super::BeginPlay();

	// save static mesh component for efficiency
	TArray<UStaticMeshComponent*> StaticMeshComponents;
	GetComponents(StaticMeshComponents);

	// make sure we found the static mesh
	if (StaticMeshComponents.Num() > 0)
	{
		// the mesh we want is at location 0 because there's only 1 mesh
		StaticMeshComponent = StaticMeshComponents[0];
	}

	// set up delegate for collisions with pickups
	StaticMeshComponent->OnComponentBeginOverlap.AddDynamic(this, &ANewton::OnOverlapBegin);

	// use UE_LOG to print current health
	UE_LOG(LogTemp, Warning, TEXT("Health: %d"),
		Health);
}

/**
 * Called every frame
 * @param DeltaTime Game time elapsed during last frame modified by the time dilation
*/
void ANewton::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

/**
 * Called when the pawn overlaps with a pickup
*/
void ANewton::OnOverlapBegin(class UPrimitiveComponent* OverlappedComp,
	class AActor* OtherActor, class UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// if the other actor has an Apple gameplay tag,
	// use its getter to add its health value to Health
	// and destroy the other actor
	if (OtherActor != nullptr &&
		OtherActor->Tags.Contains("Apple"))
	{
		AApple* Apple = (AApple*)OtherActor;
		Health += Apple->GetHealthValue();
		OtherActor->Destroy();
	}

	// use UE_LOG to print current health
	UE_LOG(LogTemp, Warning, TEXT("Health: %d"),
		Health);
}
