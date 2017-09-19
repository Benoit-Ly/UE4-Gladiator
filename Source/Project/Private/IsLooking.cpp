// Fill out your copyright notice in the Description page of Project Settings.

#include "Project.h"
#include "IsLooking.h"
#include "BehaviorTree/BlackboardComponent.h"

bool UIsLooking::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	UBlackboardComponent* BlackBoard = OwnerComp.GetBlackboardComponent();
	if (BlackBoard == nullptr)
	{
		return false;
	}

	AActor* Viewer = Cast<AActor>(BlackBoard->GetValueAsObject(ViewerKey.SelectedKeyName));
	AActor* Target = Cast<AActor>(BlackBoard->GetValueAsObject(TargetKey.SelectedKeyName));
	if (Target == nullptr)
	{
		return false;
	}

	FVector ViewerToTarget = Target->GetActorLocation() - Viewer->GetActorLocation();
	FVector ViewerLookDir = Viewer->GetActorForwardVector();

	float ViewAngleRad = FMath::Acos(ViewerToTarget.CosineAngle2D(ViewerLookDir));
	float ViewAngle = FMath::RadiansToDegrees(ViewAngleRad);

	return (ViewAngle <= Angle);
}


