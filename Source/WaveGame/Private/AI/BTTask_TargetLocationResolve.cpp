// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTTask_TargetLocationResolve.h"
#include "VisualLogger/VisualLogger.h"
#include "BehaviorTree/BlackboardComponent.h"


UBTTask_TargetLocationResolve::UBTTask_TargetLocationResolve(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	NodeName = "Run EQS Query And Pick Best Option";
	RandomNess = 10;
	QueryFinishedDelegate = FQueryFinishedSignature::CreateUObject(this, &UBTTask_TargetLocationResolve::OnQueryFinished);

}

EBTNodeResult::Type UBTTask_TargetLocationResolve::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AActor* QueryOwner = OwnerComp.GetOwner();
	AController* ControllerOwner = Cast<AController>(QueryOwner);
	if (ControllerOwner)
	{
		QueryOwner = ControllerOwner->GetPawn();
	}

	if (QueryOwner && EQSRequest.IsValid())
	{
		const UBlackboardComponent* BlackboardComponent = OwnerComp.GetBlackboardComponent();
		FBTEnvQueryTaskMemory* MyMemory = CastInstanceNodeMemory<FBTEnvQueryTaskMemory>(NodeMemory);

		MyMemory->RequestID = EQSRequest.Execute(*QueryOwner, BlackboardComponent, QueryFinishedDelegate);

		const bool bValid = (MyMemory->RequestID >= 0);
		if (bValid)
		{
			WaitForMessage(OwnerComp, UBrainComponent::AIMessage_QueryFinished, MyMemory->RequestID);
			return EBTNodeResult::InProgress;
		}
	}

	return EBTNodeResult::Failed;
}

void UBTTask_TargetLocationResolve::OnQueryFinished(TSharedPtr<FEnvQueryResult> Result)
{
	if (Result->IsAborted())
	{
		return;
	}
	AActor* MyOwner = Cast<AActor>(Result->Owner.Get());
	if (APawn* PawnOwner = Cast<APawn>(MyOwner))
	{
		MyOwner = PawnOwner->GetController();
	}
	UBehaviorTreeComponent* MyComp = MyOwner ? MyOwner->FindComponentByClass<UBehaviorTreeComponent>() : NULL;
	if (!MyComp)
	{
		UE_LOG(LogBehaviorTree, Warning, TEXT("Unable to find behavior tree to notify about finished query from %s!"), *GetNameSafe(MyOwner));
		return;
	}

	bool bSuccess = Result->IsSuccessful() && (Result->Items.Num() >= 1);
	if (bSuccess)
	{
		UBlackboardComponent* MyBlackboard = MyComp->GetBlackboardComponent();
		UEnvQueryItemType* ItemTypeCDO = Result->ItemType->GetDefaultObject<UEnvQueryItemType>();

		int32 BestIndex = 0;
		FVector BestOption = Result->GetItemAsLocation(BestIndex);
		if ((PreviousPickedLocation - BestOption).Size() <= 20.f)
		{
			BestIndex = FMath::RandRange(BestIndex + 1, RandomNess);
			BestOption = Result->GetItemAsLocation(BestIndex);
		}
		
		bSuccess = ItemTypeCDO->StoreInBlackboard(BlackboardKey, MyBlackboard, Result->RawData.GetData() + Result->Items[BestIndex].DataOffset);
		PreviousPickedLocation = BestOption;
		if (!bSuccess)
		{
			UE_VLOG(MyOwner, LogBehaviorTree, Warning, TEXT("Failed to store query result! item:%s key:%s"),
				*UEnvQueryTypes::GetShortTypeName(Result->ItemType).ToString(),
				*UBehaviorTreeTypes::GetShortTypeName(BlackboardKey.SelectedKeyType));
		}
	}
	else if (bUpdateBBOnFail)
	{
		UBlackboardComponent* MyBlackboard = MyComp->GetBlackboardComponent();
		check(MyBlackboard);
		MyBlackboard->ClearValue(BlackboardKey.GetSelectedKeyID());
	}

	// Finishing the task with whatever the status that we have.
	FAIMessage::Send(MyComp, FAIMessage(UBrainComponent::AIMessage_QueryFinished, this, Result->QueryID, bSuccess));
}
