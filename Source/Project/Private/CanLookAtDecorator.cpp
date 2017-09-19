// Fill out your copyright notice in the Description page of Project Settings.

#include "Project.h"
#include "PickableItemBase.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "CanLookAtDecorator.h"

UCanLookAtDecorator::UCanLookAtDecorator(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	NodeName = "Can Look At Decorator";
}

bool UCanLookAtDecorator::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	AController* Controller = Cast<AController>(OwnerComp.GetOwner());
	AActor* AI = (Controller != nullptr) ? Controller->GetPawn() : nullptr;

	UBlackboardComponent* BlackBoard = OwnerComp.GetBlackboardComponent();
	if (BlackBoard == nullptr || AI == nullptr)
	{
		return false;
	}

	AActor * TargetActor = Cast<AActor>(BlackBoard->GetValueAsObject(TargetKey.SelectedKeyName));
	if (!TargetActor)
		return false;

	TArray<AActor *> IgnoredActors;
	TSubclassOf<APickableItemBase> ClassToIgnore;

	UGameplayStatics::GetAllActorsOfClass(Controller->GetWorld(), ClassToIgnore, IgnoredActors);

	FHitResult HitResult;
	Trace(HitResult, IgnoredActors, AI, TargetActor);

	if (HitResult.GetActor() == TargetActor)
	{
		float ViewAngleRad = FMath::Acos(FVector::DotProduct(AI->GetActorForwardVector(), 
			TargetActor->GetActorLocation() - AI->GetActorLocation()));
		float ViewAngle = FMath::RadiansToDegrees(ViewAngleRad);
		if (ViewAngle < AngleVision)
			return true;
	}

	return false;
}

void	UCanLookAtDecorator::Trace(FHitResult& OutHit, TArray<AActor *> IgnoredActors, const AActor * From, const AActor * Target) const
{
	if (!Target)
		return;

	FCollisionQueryParams TraceParams(FName(TEXT("Can Look At Trace")), true, From);
	TraceParams.AddIgnoredActors(IgnoredActors);

	FVector Start = From->GetActorLocation() + From->GetActorForwardVector() * 100;
	FVector End = Target->GetActorLocation();

	//Re-initialize hit info
	OutHit = FHitResult(ForceInit);
	From->GetWorld()->LineTraceSingleByChannel(OutHit, Start, End, ECollisionChannel::ECC_GameTraceChannel1, TraceParams);
}


