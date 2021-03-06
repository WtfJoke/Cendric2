#pragma once

#include "global.h"
#include "Nodes/DialogueNode.h"

struct NodeTranslation;
struct NodeTrigger;

// NPC and Cendric Node
class TriggerableNode : public virtual DialogueNode {
	friend class DialogueIO;
public:
	TriggerableNode();
	virtual ~TriggerableNode();

	std::string exportToLua(int indentationLevel) const override;
	std::string exportToSQL() const override;
	std::string exportToDia(int indentationLevel) override;

	NodeTranslation* getTranslation();
	std::vector<NodeTrigger*>& getTriggers();

	// adds a new trigger with the selected trigger template
	void addTriggerTemplate();
	int currentPreselectedTrigger = 0;

protected:
	virtual std::string getTriggerableNodeType() const = 0;

	NodeTranslation* m_translation;
	std::vector<NodeTrigger*> m_triggers;
};