// Fill out your copyright notice in the Description page of Project Settings.


#include "GameOverView.h"

#include "Components/Button.h"

#include "MasterView.h"

void UGameOverView::NativeConstruct()
{
	Super::NativeConstruct();

	if (MainMenuButton)
	{
		MainMenuButton->OnClicked.AddUniqueDynamic(this, &UGameOverView::OnMainMenuPressed);
	}
}

void UGameOverView::OnMainMenuPressed()
{
	ParentViewRef->OnMainMenu.Broadcast();
}
