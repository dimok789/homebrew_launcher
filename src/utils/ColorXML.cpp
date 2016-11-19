#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tinyxml.h"
#include "utils/logger.h"
#include "ColorXML.h"

#define ENTRIE_SIZE	 8192

/* qparam filename Filepath of the XML file */
bool ColorXML::LoadColorXMLData(const char* filename) {

	TiXmlDocument xmlDoc(filename);
	if(!xmlDoc.LoadFile())
		return false;

	TiXmlElement *colorNode =  xmlDoc.FirstChildElement("color");
	if(!colorNode)
		return false;

	TiXmlElement *node = NULL;
	TiXmlElement *nodeR = NULL;
	TiXmlElement *nodeG = NULL;
	TiXmlElement *nodeB = NULL;
	TiXmlElement *nodeA = NULL;

	node = colorNode->FirstChildElement("color1");
	if(node && node->FirstChildElement("R") && node->FirstChildElement("G") && node->FirstChildElement("B") && node->FirstChildElement("A")) {
		nodeR = node->FirstChildElement("R");
		if(nodeR->FirstChild() && nodeR->FirstChild()->Value()) {
			color1.R = atoi(nodeR->FirstChild()->Value());
		}
		nodeG = node->FirstChildElement("G");
		if(nodeG->FirstChild() && nodeG->FirstChild()->Value()) {
			color1.G = atoi(nodeG->FirstChild()->Value());
		}
		nodeB = node->FirstChildElement("B");
		if(nodeB->FirstChild() && nodeB->FirstChild()->Value()) {
			color1.B = atoi(nodeB->FirstChild()->Value());
		}
		nodeA = node->FirstChildElement("A");
		if(nodeA->FirstChild() && nodeA->FirstChild()->Value()) {
			color1.A = atoi(nodeA->FirstChild()->Value());
		}
	}

	node = NULL;
	nodeR = NULL;
	nodeG = NULL;
	nodeB = NULL;
	nodeA = NULL;

	node = colorNode->FirstChildElement("color2");
	if(node && node->FirstChildElement("R") && node->FirstChildElement("G") && node->FirstChildElement("B") && node->FirstChildElement("A")) {
		nodeR = node->FirstChildElement("R");
		if(nodeR->FirstChild() && nodeR->FirstChild()->Value()) {
			color2.R = atoi(nodeR->FirstChild()->Value());
		}
		nodeG = node->FirstChildElement("G");
		if(nodeG->FirstChild() && nodeG->FirstChild()->Value()) {
			color2.G = atoi(nodeG->FirstChild()->Value());
		}
		nodeB = node->FirstChildElement("B");
		if(nodeB->FirstChild() && nodeB->FirstChild()->Value()) {
			color2.B = atoi(nodeB->FirstChild()->Value());
		}
		nodeA = node->FirstChildElement("A");
		if(nodeA->FirstChild() && nodeA->FirstChild()->Value()) {
			color2.A = atoi(nodeA->FirstChild()->Value());
		}
	}

	node = NULL;
	nodeR = NULL;
	nodeG = NULL;
	nodeB = NULL;
	nodeA = NULL;

	node = colorNode->FirstChildElement("color3");
	if(node && node->FirstChildElement("R") && node->FirstChildElement("G") && node->FirstChildElement("B") && node->FirstChildElement("A")) {
		nodeR = node->FirstChildElement("R");
		if(nodeR->FirstChild() && nodeR->FirstChild()->Value()) {
			color3.R = atoi(nodeR->FirstChild()->Value());
		}
		nodeG = node->FirstChildElement("G");
		if(nodeG->FirstChild() && nodeG->FirstChild()->Value()) {
			color3.G = atoi(nodeG->FirstChild()->Value());
		}
		nodeB = node->FirstChildElement("B");
		if(nodeB->FirstChild() && nodeB->FirstChild()->Value()) {
			color3.B = atoi(nodeB->FirstChild()->Value());
		}
		nodeA = node->FirstChildElement("A");
		if(nodeA->FirstChild() && nodeA->FirstChild()->Value()) {
			color3.A = atoi(nodeA->FirstChild()->Value());
		}
	}

	node = NULL;
	nodeR = NULL;
	nodeG = NULL;
	nodeB = NULL;
	nodeA = NULL;

	node = colorNode->FirstChildElement("color2");
	if(node && node->FirstChildElement("R") && node->FirstChildElement("G") && node->FirstChildElement("B") && node->FirstChildElement("A")) {
		nodeR = node->FirstChildElement("R");
		if(nodeR->FirstChild() && nodeR->FirstChild()->Value()) {
			color4.R = atoi(nodeR->FirstChild()->Value());
		}
		nodeG = node->FirstChildElement("G");
		if(nodeG->FirstChild() && nodeG->FirstChild()->Value()) {
			color4.G = atoi(nodeG->FirstChild()->Value());
		}
		nodeB = node->FirstChildElement("B");
		if(nodeB->FirstChild() && nodeB->FirstChild()->Value()) {
			color4.B = atoi(nodeB->FirstChild()->Value());
		}
		nodeA = node->FirstChildElement("A");
		if(nodeA->FirstChild() && nodeA->FirstChild()->Value()) {
			color4.A = atoi(nodeA->FirstChild()->Value());
		}
	}
	return true;
}


ColorData ColorXML::GetColor1() {
	return color1;
}

ColorData ColorXML::GetColor2() {
	return color2;
}

ColorData ColorXML::GetColor3() {
	return color3;
}

ColorData ColorXML::GetColor4() {
	return color4;
}
