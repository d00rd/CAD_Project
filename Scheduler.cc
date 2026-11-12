//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
// 

#include "Scheduler.h"

Define_Module(Scheduler);



Scheduler::Scheduler()
{
    selfMsg = nullptr;
}

Scheduler::~Scheduler()
{
    cancelAndDelete(selfMsg);
}


void Scheduler::initialize()
{
    NrUsers = par("gateSize").intValue();
    NrOfChannels = 10;//read from omnetpp.ini
    selfMsg = new cMessage("selfMsg");
    for(int i=0; i<10;i++){
           q[i]=0;
           NrBlocks[i]=0;
    }
    scheduleAt(simTime(), selfMsg);


}

void Scheduler::RoundRobin(int user, int channels, int checked)
{
    if(channels == 0 || checked == NrUsers)
        return;

    user = user % NrUsers;

    if(q[user] > 0) {
        int allocated = std::min(q[user], channels);

        NrBlocks[user] += allocated;
        q[user] -= allocated;

        NextUser = (user + 1) % NrUsers;

        RoundRobin(NextUser, channels - allocated, 0);
    } else {
        RoundRobin((user + 1) % NrUsers, channels, checked + 1);
    }
}


int Scheduler::best_quality(){
    int quality = -1;
    int user = -1;
    for(int i = 0; i < NrUsers; i++) {
        if(q[i] != 0 && r[i] > quality && NrBlocks[i] != q[i]) {
            quality = r[i];
            user = i;
        }
    }
    return user;
}
void Scheduler::Opportunistic() {

    int channelsLeft = NrOfChannels;

    while(channelsLeft > 0) {

        int user = best_quality();
        if(user == -1)
            break;

        int allocated = std::min(q[user], channelsLeft);
        NrBlocks[user] += allocated;
        q[user] -= allocated;
        channelsLeft -= allocated;
    }
}



void Scheduler::handleMessage(cMessage *msg)
{
   // int q[NrUsers];
 //   int NrBlocks[Nrusers];

    /*
    for(int j=0;j<NrUsers; j++){
        q[j]= getParentModule()->getSubmodule("user",j)->getSubmodule("myqq")->par("qlp");
        EV << "q["<<j<<"]= " << q[j] <<endl;
    }
     */


    int userWeights[NrUsers];
    for(int i =0; i<NrUsers;i++){
        if (msg->arrivedOn("rxInfo", i)){
            q[i]= msg->par("ql_info");
            EV << "Update: q["<<i<<"]= " << q[i] <<endl;
            delete(msg);
    }
    }

        if (msg == selfMsg){

          //here comes the scheduling algorithm !!!
//            for(int i =0;i<NrUsers;i++){
//                NrBlocks[i]=4;//you have to change this !!!
//             }
            //RoundRobin(NextUser, NrOfChannels, 0);
            Opportunistic();
            for(int i =0;i<NrUsers;i++){

        //finds out the length of each queue !!
                if(NrBlocks[i]>0){
                    cMessage *cmd = new cMessage("cmd");

                    //q[i]= q[i]-NrBlocks[i];
                    // EV << "Decrease: q["<<i<<"]= " << q[i] <<endl;
                    cmd->addPar("nrBlocks");
                    cmd->par("nrBlocks").setLongValue(NrBlocks[i]);
                    //set parameter value, e.g., nr of blocks to be sent from the queue by user i
                    //
                    send(cmd,"txScheduling",i);
                    NrBlocks[i] = 0;
                }
          //  double xtime = simTime().dbl();
            }
        scheduleAt(simTime()+par("schedulingPeriod").doubleValue(), selfMsg);

    }

}
