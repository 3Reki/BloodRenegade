// Fill out your copyright notice in the Description page of Project Settings.
#include "DialogueSessionNode.h"
#include "DialogueSession.h"

#define LOCTEXT_NAMESPACE "DialogueSessionNode"

UDialogueSessionNode::UDialogueSessionNode()
{
#if WITH_EDITORONLY_DATA
    CompatibleGraphType = UDialogueSession::StaticClass();

    ContextMenuName = LOCTEXT("ConextMenuName", "Dialogue Session Node");
#endif
}

#if WITH_EDITOR

FText UDialogueSessionNode::GetNodeTitle() const
{
    return Paragraph.IsEmpty() ? LOCTEXT("EmptyParagraph", "(Empty paragraph)") : Paragraph;
}

void UDialogueSessionNode::SetNodeTitle(const FText& NewTitle)
{
    Paragraph = NewTitle;
}

FLinearColor UDialogueSessionNode::GetBackgroundColor() const
{
    return FLinearColor::Red;
}

#endif

#undef LOCTEXT_NAMESPACE