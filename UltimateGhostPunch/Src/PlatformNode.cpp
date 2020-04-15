#include "PlatformNode.h"

#include <GaiaData.h>
#include <sstream>

PlatformNode::PlatformNode() : iniPos(Vector3()), endPos(Vector3()), midPos(Vector3()), index(-1)
{
	edges = std::vector<NavigationLink>();
}

PlatformNode::PlatformNode(const Vector3& iniPos, int index) : iniPos(iniPos), endPos(iniPos), midPos(iniPos), index(index)
{
	edges = std::vector<NavigationLink>();
}

PlatformNode::PlatformNode(const Vector3& iniPos, const Vector3& endPos, int index) : iniPos(iniPos), endPos(endPos), index(index)
{
	edges = std::vector<NavigationLink>();
	midPos = (iniPos + endPos) / 2.0f;
}

PlatformNode::~PlatformNode()
{
	edges.clear();
}

void PlatformNode::setEnd(const Vector3& endPos)
{
	this->endPos = endPos;
}

void PlatformNode::setBegining(const Vector3& iniPos)
{
	this->iniPos = iniPos;
}

Vector3 PlatformNode::getBegining() const
{
	return iniPos;
}

Vector3 PlatformNode::getEnd() const
{
	return endPos;
}

Vector3 PlatformNode::getMiddle() const
{
	return midPos;
}

int PlatformNode::getIndex() const
{
	return index;
}

void PlatformNode::addEdge(const NavigationLink& link)
{
	edges.push_back(link);
}

void PlatformNode::removeLastEdge()
{
	if(!edges.empty())
		edges.pop_back();
}

void PlatformNode::removeAllEdges()
{
	edges.clear();
}

GaiaData PlatformNode::savePlatform()
{
	GaiaData platform;
	platform.addElement<std::string>("index", std::to_string(index));

	platform.addElement<std::string>("iniPos", iniPos.toString());
	platform.addElement<std::string>("endPos", endPos.toString());

	std::vector<GaiaData>links;
	for (NavigationLink link : edges)
		links.push_back(link.saveLink());
	platform.addElement("edges", links);
	return platform;
}

void PlatformNode::loadPlatform(const GaiaData& data)
{
	std::stringstream ss;

	ss = std::stringstream(data.find("index").getValue());
	ss >> index;

	ss = std::stringstream(data.find("iniPos").getValue());
	ss >> iniPos.x >> iniPos.y >> iniPos.z;

	ss = std::stringstream(data.find("endPos").getValue());
	ss >> endPos.x >> endPos.y >> endPos.z;

	GaiaData links = data.find("edges");
	for (GaiaData::iterator it = links.begin(); it != links.end(); it++) {
		NavigationLink link;
		link.loadLink((*it));

		edges.push_back(link);
	}

	midPos = (iniPos + endPos) / 2.0f;
}

std::vector<NavigationLink> PlatformNode::getEdges()
{
	return edges;
}


//STATE
State::State() : frame(-1), action(Action::None), pos(Vector3())
{

}

State::State(Action action, int frame, const Vector3& pos) : frame(frame), action(action), pos(pos)
{
}

Vector3 State::getPos()
{
	return pos;
}

GaiaData State::saveState()
{
	GaiaData state;
	state.addElement<std::string>("action", std::to_string((int)action));
	state.addElement<std::string>("frame", std::to_string(frame));
	state.addElement<std::string>("position", pos.toString());
	return state;
}

void State::loadState(const GaiaData& data)
{
	std::stringstream ss;

	int aux;
	ss = std::stringstream(data.find("action").getValue());
	ss >> aux;
	action = (Action)aux;

	ss = std::stringstream(data.find("frame").getValue());
	ss >> frame;

	ss = std::stringstream(data.find("position").getValue());
	ss >> pos.x>>pos.y>>pos.z;
}

//NAVIGATION LINK
NavigationLink::NavigationLink() : connection(-1), linkStates(std::vector<State>()), frames(0), iniPos(Vector3()), endPos(Vector3())
{
}

NavigationLink::NavigationLink(const std::vector<State>& states, const Vector3& iniPos, const Vector3& endPos, int frames, int connection) : linkStates(states), connection(connection), iniPos(iniPos), endPos(endPos), frames(frames)
{
}

int NavigationLink::getConnection() const
{
	return connection;
}

std::vector<State> NavigationLink::getStates() const
{
	return linkStates;
}

Vector3 NavigationLink::getIniPos() const
{
	return iniPos;
}

Vector3 NavigationLink::getEndPos() const
{
	return endPos;
}

int NavigationLink::getFrames() const
{
	return frames;
}

GaiaData NavigationLink::saveLink()
{
	GaiaData link;
	link.addElement<std::string>("connection", std::to_string(connection));
	link.addElement<std::string>("frames", std::to_string(frames));
	link.addElement<std::string>("iniPos", iniPos.toString());
	link.addElement<std::string>("endPos", endPos.toString());
	std::vector<GaiaData>states;
	for (State s : linkStates)
		states.push_back(s.saveState());
	link.addElement("states", states);
	return link;
}

void NavigationLink::loadLink(const GaiaData& data)
{
	std::stringstream ss;

	ss = std::stringstream(data.find("connection").getValue());
	ss >> connection;

	ss = std::stringstream(data.find("frames").getValue());
	ss >> connection;

	ss = std::stringstream(data.find("iniPos").getValue());
	ss >> iniPos.x >> iniPos.y >> iniPos.z;

	ss = std::stringstream(data.find("endPos").getValue());
	ss >> endPos.x >> endPos.y >> endPos.z;

	GaiaData states = data.find("states");
	for (GaiaData::iterator it = states.begin(); it != states.end(); it++) {
		State state;
		state.loadState((*it));

		linkStates.push_back(state);
	}
}