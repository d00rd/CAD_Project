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
import User;
import Sink;
import Scheduler;

network RadioSch2
{
    parameters:
        int usersCount;
//= default(3);

        @display("bgb=350,256");
    submodules:
        scheduler: Scheduler {
            gateSize = parent.usersCount;
            @display("p=48,117");
        }
        sink: Sink {
            gateSize = parent.usersCount;
            @display("p=300,133");
        }
        user[usersCount]: User {
        }
    connections:
        for i=0..usersCount - 1 {
            scheduler.txScheduling[i] --> user[i].rxScheduling;
            user[i].txPackets --> sink.rxPackets[i];
            user[i].txInfo --> scheduler.rxInfo[i];

        }
}
