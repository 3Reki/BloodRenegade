#include "DialogueSession.h"
#include "DialogueSessionNode.h"
#include "DialogueSessionEdge.h"

#define LOCTEXT_NAMESPACE "DialogueSession"

UDialogueSession::UDialogueSession()
{
    NodeType = UDialogueSessionNode::StaticClass();
    EdgeType = UDialogueSessionEdge::StaticClass();

    Name = "DialogueSession";
}

bool UDialogueSession::CanPlayDialogue() 
{
    bool result;
    int chance;
   
    if (RecentActivation >= 0) 
    {
        chance = (1 + RecentActivation) * ActivationPercentage;
        result = FMath::RandRange(0, 100) < (chance);
    }
    else 
    {
        chance = ActivationPercentage / -RecentActivation;
        result = FMath::RandRange(0, 100) < (ActivationPercentage / (-RecentActivation + 1));
    }
    
    if (result) RecentActivation--;
    else RecentActivation++;

    return result;
}

void UDialogueSession::ResetRecentActivation()
{
    RecentActivation = 0;
}

#undef LOCTEXT_NAMESPACE
