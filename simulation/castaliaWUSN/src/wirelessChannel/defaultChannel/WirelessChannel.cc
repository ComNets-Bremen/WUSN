/****************************************************************************
 *  Copyright: National ICT Australia,  2007 - 2010                         *
 *  Developed at the ATP lab, Networked Systems research theme              *
 *  Author(s): Athanassios Boulis, Yuriy Tselishchev                        *
 *  This file is distributed under the terms in the attached LICENSE file.  *
 *  If you do not find this file, copies can be found by writing to:        *
 *                                                                          *
 *      NICTA, Locked Bag 9013, Alexandria, NSW 1435, Australia             *
 *      Attention:  License Inquiry.                                        *
 *                                                                          *
 ****************************************************************************/

#include "wirelessChannel/defaultChannel/WirelessChannel.h"

Define_Module(WirelessChannel);

int WirelessChannel::numInitStages() const
{
    return 2;
}

void WirelessChannel::initialize(int stage)
{
    if (stage == 0) {
        readIniFileParameters();
        return;
    }

    packetlossprob = par("packetlossprob");
    randnoise = par("randnoise");
    distance_w = par("distance_w");
    sand_w = par("sand_w");
    clay_w = par("clay_w");
    freq_w = par("freq_w");

    /* variable to report initialization run time */
    clock_t startTime;
    startTime = clock();

    /****************************************************
     * To handle mobile nodes we break up the space into
     * cells. All path loss calculations are now done
     * between cells. First we need to find out how many
     * distinct cells we have in space, based on the
     * dimensions of the space and the declared cell size.
     * If we only have static nodes, then we keep the
     * same variables and abstractions to make the code
     * more compact and easier to read, but we do not
     * really need to divide the space into cells. Even
     * though we keep the variable names, cells in the
     * static case only correspond to node positions
     * and we only have numOfNodes cells, much like
     * we used to do in Castalia 1.3
     ****************************************************/

    if (onlyStaticNodes) {
        numOfSpaceCells = numOfNodes;
    } else {
        if (xFieldSize <= 0) {
            xFieldSize = 1;
            xCellSize = 1;
        }
        if (yFieldSize <= 0) {
            yFieldSize = 1;
            yCellSize = 1;
        }
        if (zFieldSize <= 0) {
            zFieldSize = 1;
            zCellSize = 1;
        }
        if (xCellSize <= 0)
            xCellSize = xFieldSize;
        if (yCellSize <= 0)
            yCellSize = yFieldSize;
        if (zCellSize <= 0)
            zCellSize = zFieldSize;

        numOfZCells = (int)ceil(zFieldSize / zCellSize);
        numOfYCells = (int)ceil(yFieldSize / yCellSize);
        numOfXCells = (int)ceil(xFieldSize / xCellSize);
        numOfSpaceCells = numOfZCells * numOfYCells * numOfXCells;

        /***************************************************************
         * Calculate some values that  help us transform a 1D index in
         * [0..numOfSpaceCells -1] to a 3D index x, y, z and vice versa.
         * Each variable holds index increments (in the 1D large index)
         * needed to move one space cell in the z, y, and x directions
         **************************************************************/
        zIndexIncrement = numOfYCells * numOfXCells;
        yIndexIncrement = numOfXCells;
        xIndexIncrement = 1;
    }

    /***************************************************************
     * Allocate and initialize cellOccupation and nodeLocation.
     * nodeLocation keeps the state about all nodes locations and
     * cellOccupation is an array of lists. List at index i contains
     * the node IDs that reside in cell i. We define and use these
     * arrays even for the static nodes case as it makes the code
     * more compact and easier to follow.
     **************************************************************/
    nodeLocation = new NodeLocation_type[numOfNodes];
    if (nodeLocation == NULL)
        throw cRuntimeError("Could not allocate array nodeLocation\n");

    cellOccupation = new list<int>[numOfSpaceCells];
    if (cellOccupation == NULL)
        throw cRuntimeError("Could not allocate array cellOccupation\n");

    cTopology *topo;    // temp variable to access initial location of the nodes
    topo = new cTopology("topo");
    topo->extractByNedTypeName(cStringTokenizer("node.Node").asVector());

    for (int i = 0; i < numOfNodes; i++) {
        VirtualMobilityManager *nodeMobilityModule =
                check_and_cast<VirtualMobilityManager*>
        (topo->getNode(i)->getModule()->getSubmodule("MobilityManager"));
        nodeLocation[i] = nodeMobilityModule->getLocation();
        nodeLocation[i].cell = i;

        if (!onlyStaticNodes) {
            /******************************************************************
             * Compute the cell this node is in and initialize cellOccupation.
             * Cavaet in computing the XYZ indices:
             * Because we allow cell resolutions that do not perfectly divide
             * the field (we take the ceiling of the division when calculating
             * numOfXCells) this means that the edge cells might be smaller than
             * the others. So in some cases, the calculation we are doing
             * below, might give the wrong cell by +1. That's why we are doing
             * the test immediately after.
             ******************************************************************/
            int xIndex = (int)floor(nodeLocation[i].x / xFieldSize * numOfXCells);
            if (((xIndex - 1) * xCellSize) >= nodeLocation[i].x)
                xIndex--;
            else if (xIndex >= numOfXCells) {
                xIndex = numOfXCells - 1;   // the maximum possible x index
                if (nodeLocation[i].x > xFieldSize)
                    trace() << "WARNING at initialization: node position out of bounds in X dimension!\n";
            }

            int yIndex = (int)floor(nodeLocation[i].y / yFieldSize * numOfYCells);
            if (((yIndex - 1) * yCellSize) >= nodeLocation[i].y)
                yIndex--;
            else if (yIndex >= numOfYCells) {
                yIndex = numOfYCells - 1;   // the maximum possible y index
                if (nodeLocation[i].y > yFieldSize)
                    trace() << "WARNING at initialization: node position out of bounds in Y dimension!\n";
            }

            int zIndex = (int)floor(nodeLocation[i].z / zFieldSize * numOfZCells);
            if (((zIndex - 1) * zCellSize) >= nodeLocation[i].z)
                zIndex--;
            else if (zIndex >= numOfZCells) {
                zIndex = numOfZCells - 1;   // the maximum possible z index
                if (nodeLocation[i].z > zFieldSize)
                    trace() << "WARNING at initialization: node position out of bounds in Z dimension!\n";
            }

            int cell = zIndex * zIndexIncrement + yIndex * yIndexIncrement + xIndex * xIndexIncrement;
            if (cell < 0 || cell >= numOfSpaceCells) {
                throw cRuntimeError("Cell out of bounds for node %i, please check your mobility module settings\n", i);
            }

            nodeLocation[i].cell = cell;
        }

        /*************************************************
         * pushing ID i into the list cellOccupation[cell]
         * (if onlyStaticNodes cell=i )
         *************************************************/
        cellOccupation[nodeLocation[i].cell].push_front(i);
    }
    delete(topo);

    /**********************************************
     * Allocate and initialize the pathLoss array.
     * This is the "propagation map" of our space.
     **********************************************/
    pathLoss = new list<PathLossElement*>[numOfSpaceCells];
    if (pathLoss == NULL)
        throw cRuntimeError("Could not allocate array pathLoss\n");

    int elementSize = sizeof(PathLossElement) + 3 * sizeof(PathLossElement *);
    int totalElements = 0;  //keep track of pathLoss size for reporting purposes

    float x1, x2, y1, y2, z1, z2, dist;
    float PLd;      // path loss at distance dist, in dB
    float bidirectionalPathLossJitter; // variation of the pathloss in the two directions of a link, in dB

    /*******************************************************
     * Calculate the distance, beyond which we cannot
     * have connectivity between two nodes. This calculation is
     * based on the maximum TXPower the signalDeliveryThreshold
     * the pathLossExponent, the PLd0. For the random
     * shadowing part we use 3*sigma to account for 99.7%
     * of the cases. We use this value to considerably
     * speed up the filling of the pathLoss array,
     * especially for the mobile case.
     *******************************************************/
    float distanceThreshold = d0 *
            pow(10.0,(maxTxPower - signalDeliveryThreshold - PLd0 + 3 * sigma) /
                    (10.0 * pathLossExponent));

    for (int i = 0; i < numOfSpaceCells; i++) {
        if (onlyStaticNodes) {
            x1 = nodeLocation[i].x;
            y1 = nodeLocation[i].y;
            z1 = nodeLocation[i].z;
        } else {
            z1 = zCellSize * (int)floor(i / zIndexIncrement);
            y1 = yCellSize * (((int)floor(i / yIndexIncrement)) % zIndexIncrement);
            x1 = xCellSize * (((int)floor(i / xIndexIncrement)) % yIndexIncrement);
        }

        /* Path loss to yourself is 0.0 */
        pathLoss[i].push_front(new PathLossElement(i, 0.0));
        totalElements++;    //keep track of pathLoss size for reporting purposes

        for (int j = i + 1; j < numOfSpaceCells; j++) {
            if (onlyStaticNodes) {
                x2 = nodeLocation[j].x;
                y2 = nodeLocation[j].y;
                z2 = nodeLocation[j].z;
            } else {
                z2 = zCellSize * (int)(j / zIndexIncrement);
                y2 = yCellSize * (((int)(j / yIndexIncrement)) % zIndexIncrement);
                x2 = xCellSize * (((int)(j / xIndexIncrement)) % yIndexIncrement);

                if (fabs(x1 - x2) > distanceThreshold)
                    continue;
                if (fabs(y1 - y2) > distanceThreshold)
                    continue;
                if (fabs(z1 - z2) > distanceThreshold)
                    continue;
            }

            dist = sqrt((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1) + (z2 - z1) * (z2 - z1));
            if (dist > distanceThreshold)
                continue;

            /* if the distance is very small (arbitrarily: smaller than one tenth
             * of the reference distance) then make the path loss 0dB
             */
            if (dist < d0/10.0) {
                PLd = 0;
                bidirectionalPathLossJitter = 0;
            }
            else {
                PLd = PLd0 + 10.0 * pathLossExponent * log10(dist / d0) + normal(0, sigma);
                bidirectionalPathLossJitter = normal(0, bidirectionalSigma) / 2;
            }

            if (maxTxPower - PLd - bidirectionalPathLossJitter >= signalDeliveryThreshold) {
                pathLoss[i].push_front(new PathLossElement(j,PLd + bidirectionalPathLossJitter));
                totalElements++;    //keep track of pathLoss size for reporting purposes
            }

            if (maxTxPower - PLd + bidirectionalPathLossJitter >= signalDeliveryThreshold) {
                pathLoss[j].push_front(new PathLossElement(i,PLd - bidirectionalPathLossJitter));
                totalElements++;    //keep track of pathLoss size for reporting purposes
            }
        }
    }

    trace() << "Number of distinct space cells: " << numOfSpaceCells;
    trace() << "Each cell affects " <<
            (double)totalElements / numOfSpaceCells << " other cells on average";
    trace() << "The pathLoss array of lists was allocated in " <<
            (double)(totalElements * elementSize) / 1048576 << " MBytes";
    // The larger this number, the slower your simulation. Consider increasing the cell size,
    // decreasing the field size, or if you only have static nodes, decreasing the number of nodes

    /*********************************************************************
     * Allocate nodesAffectedByTransmitter even for static nodes.
     * This makes the code more compact. We also have temporal variations
     * so the nodes that are affected are not necessarily the same.
     *********************************************************************/
    nodesAffectedByTransmitter = new list<int>[numOfNodes];
    if (nodesAffectedByTransmitter == NULL)
        throw cRuntimeError("Could not allocate array nodesAffectedByTransmitter\n");

    /************************************************************
     * If direct assignment of link qualities is given at the
     * omnetpp.ini file we parse the input and update pathLoss.
     * This is only for static nodes. (onlyStaticNodes==TRUE)
     ************************************************************/
    parsePathLossMap();

    /* Create temporal model object from parameters file (if given) */
    if (strlen(temporalModelParametersFile) > 0) {
        temporalModel = new channelTemporalModel(temporalModelParametersFile, getRNG(2));
        temporalModelDefined = true;
    } else {
        temporalModelDefined = false;
    }

    declareHistogram("Fade depth distribution", -50, 15, 13);

    trace() << "Time for Wireless Channel module initialization: " <<
            (double)(clock() - startTime) / CLOCKS_PER_SEC << "secs";
}

/*****************************************************************************
 * This is where the main work is done by processing all the messages received
 *****************************************************************************/
void WirelessChannel::handleMessage(cMessage * msg)
{
    switch (msg->getKind()) {

    case WC_NODE_MOVEMENT:{

        WirelessChannelNodeMoveMessage *mobilityMsg =
                check_and_cast <WirelessChannelNodeMoveMessage*>(msg);
        int srcAddr = mobilityMsg->getNodeID();

        /*****************************************************
         * A node notified the wireless channel that it moved
         * to a new space cell. Update the nodeLocation and
         * based on the new cell calculation decide if the
         * cellOccupation array needs to be updated.
         *****************************************************/

        if (onlyStaticNodes)
            throw cRuntimeError("Error: Rerceived WS_NODE_MOVEMENT msg, while onlyStaticNodes is TRUE");

        int oldCell = nodeLocation[srcAddr].cell;
        nodeLocation[srcAddr].x = mobilityMsg->getX();
        nodeLocation[srcAddr].y = mobilityMsg->getY();
        nodeLocation[srcAddr].z = mobilityMsg->getZ();
        nodeLocation[srcAddr].phi = mobilityMsg->getPhi();
        nodeLocation[srcAddr].theta = mobilityMsg->getTheta();
        if ((nodeLocation[srcAddr].x < 0.0) ||
                (nodeLocation[srcAddr].y < 0.0) ||
                (nodeLocation[srcAddr].z < 0.0))
            throw cRuntimeError("Wireless channel received faulty WC_NODE_MOVEMENT msg. We cannot have negative node coordinates");

        int xIndex = (int)floor(nodeLocation[srcAddr].x / xFieldSize * numOfXCells);
        if (((xIndex - 1) * xCellSize) >= nodeLocation[srcAddr].x)
            xIndex--;
        else if (xIndex >= numOfXCells) {
            xIndex = numOfXCells - 1;   // the maximum possible x index
            if (nodeLocation[srcAddr].x > xFieldSize)
                debug() << "WARNING at WC_NODE_MOVEMENT: node position out of bounds in X dimension!\n";
        }

        int yIndex = (int)floor(nodeLocation[srcAddr].y / yFieldSize * numOfYCells);
        if (((yIndex - 1) * yCellSize) >= nodeLocation[srcAddr].y)
            yIndex--;
        else if (yIndex >= numOfYCells) {
            yIndex = numOfYCells - 1;   // the maximum possible y index
            if (nodeLocation[srcAddr].y > yFieldSize)
                debug() << "WARNING at WC_NODE_MOVEMENT: node position out of bounds in Y dimension!\n";
        }

        int zIndex = (int)floor(nodeLocation[srcAddr].z / zFieldSize * numOfZCells);
        if (((zIndex - 1) * zCellSize) >= nodeLocation[srcAddr].z)
            zIndex--;
        else if (zIndex >= numOfZCells) {
            zIndex = numOfZCells - 1;   // the maximum possible z index
            if (nodeLocation[srcAddr].z > zFieldSize)
                debug() << "WARNING at WC_NODE_MOVEMENT: node position out of bounds in Z dimension!\n";
        }

        int newCell = zIndex * zIndexIncrement + yIndex * yIndexIncrement + xIndex * xIndexIncrement;
        if (newCell != oldCell) {
            cellOccupation[oldCell].remove(srcAddr);
            cellOccupation[newCell].push_front(srcAddr);
            nodeLocation[srcAddr].cell = newCell;
        }

        break;
    }

    case WC_UPDATE_VWC:{



        //   EV <<"Wireless channel update due to VWC msg received"<< "\n" ;

        WirelessChannelUpdateMessage *VWCMsg =
                check_and_cast <WirelessChannelUpdateMessage*>(msg);
        double VWC;

        VWC = VWCMsg->getPLd0_VWC();

        float  PLd;
        double normal_noise;
        double randnoisemin,randnoisemax;

        if(randnoise==1)
        {
            randnoisemin=-0.5;
            randnoisemax=0.5;
            normal_noise = randnum(randnoisemin,randnoisemax);
        } else if(randnoise==2)
        {
            randnoisemin=-1;
            randnoisemax=1;
            normal_noise = randnum(randnoisemin,randnoisemax);
        } else if(randnoise==3)
        {
            randnoisemin=-1.5;
            randnoisemax=1.5;
            normal_noise = randnum(randnoisemin,randnoisemax);
        } else if(randnoise==4)
        {
            randnoisemin=-2;
            randnoisemax=2;
            normal_noise = randnum(randnoisemin,randnoisemax);
        } else if(randnoise==5)
        {
            randnoisemin=-2.5;
            randnoisemax=2.5;
            normal_noise = randnum(randnoisemin,randnoisemax);
        }



        // calcpathloss(double distance, double sand, double clay, double freq, double moisture, double Ptransmit )
        EV << "VWC in Wireless Channel  " << VWC <<"\n";

        PLd =  calcpathloss(distance_w,sand_w,clay_w,freq_w,VWC,10);

        //PLd=65;

        EV << "Path Loss " << PLd <<"\n";


      /*  if (uniform(0, 1) < packetlossprob)
        {
            PLd=150.0;
            EV <<  "Path loss equal to 100" <<"\n";
        }
*/
        updatePathLossElement(VWCMsg->getNodeID(), 0, PLd);


        break;
    }

    case WC_SIGNAL_START:{

        WirelessChannelSignalBegin *signalMsg =
                check_and_cast <WirelessChannelSignalBegin*>(msg);
        int srcAddr = signalMsg->getNodeID();
        int receptioncount = 0;

        /* Find the cell that the transmitting node resides */
        int cellTx = nodeLocation[srcAddr].cell;

        /* Iterate through the list of cells that are affected
         * by cellTx and check if there are nodes there.
         * Update the nodesAffectedByTransmitter array
         */
        list < PathLossElement * >::iterator it1;
        for (it1 = pathLoss[cellTx].begin(); it1 != pathLoss[cellTx].end(); it1++) {
            /* If no nodes exist in this cell, move on. */
            if (cellOccupation[(*it1)->cellID].empty())
                continue;

            /* Otherwise there are some nodes in that cell.
             * Calculate the signal received by these nodes
             * It is exactly the same for all of them.
             * The signal may be variable in time.
             */
            float currentSignalReceived = signalMsg->getPower_dBm() - (*it1)->avgPathLoss;
            if (temporalModelDefined) {
                simtime_t timePassed_msec = (simTime() - (*it1)->lastObservationTime) * 1000;
                simtime_t timeProcessed_msec =
                        temporalModel->runTemporalModel(SIMTIME_DBL(timePassed_msec),
                                &((*it1)->lastObservedDiffFromAvgPathLoss));
                currentSignalReceived += (*it1)->lastObservedDiffFromAvgPathLoss;
                collectHistogram("Fade depth distribution",
                        (*it1)->lastObservedDiffFromAvgPathLoss);
                /* Update the observation time */
                (*it1)->lastObservationTime = simTime() -
                        (timePassed_msec - timeProcessed_msec) / 1000;
            }

            /* If the resulting current signal received is not strong enough,
             * to be delivered to the radio module, continue to the next cell.
             */
            if (currentSignalReceived < signalDeliveryThreshold)
                continue;

            /* Else go through all the nodes of that cell.
             * Iterator it2 returns node IDs.
             */
            list < int >::iterator it2;
            for (it2 = cellOccupation[(*it1)->cellID].begin();
                    it2 != cellOccupation[(*it1)->cellID].end(); it2++) {
                if (*it2 == srcAddr)
                    continue;
                receptioncount++;
                WirelessChannelSignalBegin *signalMsgCopy = signalMsg->dup();
                signalMsgCopy->setPower_dBm(currentSignalReceived);
                send(signalMsgCopy, "toNode", *it2);
                nodesAffectedByTransmitter[srcAddr].push_front(*it2);
            }   //for it2
        }   //for it1

        if (receptioncount > 0)
            trace() << "signal from node[" << srcAddr << "] reached " <<
            receptioncount << " other nodes";
        break;
    }

    case WC_SIGNAL_END:{
        WirelessChannelSignalEnd *signalMsg =
                check_and_cast <WirelessChannelSignalEnd*>(msg);
        int srcAddr = signalMsg->getNodeID();

        /* Go through the list of nodes that were affected
         *  by this transmission. *it1 holds the node ID
         */
        list <int>::iterator it1;
        for (it1 = nodesAffectedByTransmitter[srcAddr].begin();
                it1 != nodesAffectedByTransmitter[srcAddr].end(); it1++) {
            WirelessChannelSignalEnd *signalMsgCopy = signalMsg->dup();
            send(signalMsgCopy, "toNode", *it1);
        }   //for it1

        /* Now that we are done processing the msg we delete the whole list
         * nodesAffectedByTransmitter[srcAddr], since srcAddr in not TXing anymore.
         */
        nodesAffectedByTransmitter[srcAddr].clear();
        break;
    }

    default:{
        throw cRuntimeError("ERROR: Wireless Channel received unknown message kind=%i", msg->getKind());
        break;
    }
    }
    delete msg;
}

void WirelessChannel::finishSpecific()
{

    /*****************************************************
     * Delete dynamically allocated arrays. Some allocate
     * lists of objects so they need an extra nested loop
     * to properly deallocate all these objects
     *****************************************************/

    /* delete pathLoss */
    for (int i = 0; i < numOfSpaceCells; i++) {
        list <PathLossElement*>::iterator it1;
        for (it1 = pathLoss[i].begin(); it1 != pathLoss[i].end(); it1++) {
            delete(*it1);   // deallocate the memory occupied by the object
        }
    }
    delete[]pathLoss;   // the delete[] operator releases memory allocated with new []

    /* delete nodesAffectedByTransmitter */
    delete[]nodesAffectedByTransmitter; // the delete[] operator releases memory allocated with new []

    /* delete cellOccupation */
    delete[]cellOccupation; // the delete[] operator releases memory allocated with new []

    /* delete nodeLocation */
    delete[]nodeLocation;

    if (temporalModelDefined)
        delete temporalModel;

    //close the output stream that CASTALIA_DEBUG is writing to
    DebugInfoWriter::closeStream();
}

void WirelessChannel::readIniFileParameters(void)
{
    DebugInfoWriter::setDebugFileName(
            getParentModule()->par("debugInfoFileName").stringValue());

    onlyStaticNodes = par("onlyStaticNodes");
    pathLossExponent = par("pathLossExponent");
    sigma = par("sigma");
    bidirectionalSigma = par("bidirectionalSigma");
    PLd0 = par("PLd0");
    d0 = par("d0");

    pathLossMapFile = par("pathLossMapFile");
    temporalModelParametersFile = par("temporalModelParametersFile");
    signalDeliveryThreshold = par("signalDeliveryThreshold");

    numOfNodes = getParentModule()->par("numNodes");
    xFieldSize = getParentModule()->par("field_x");
    yFieldSize = getParentModule()->par("field_y");
    zFieldSize = getParentModule()->par("field_z");
    xCellSize = par("xCellSize");
    yCellSize = par("yCellSize");
    zCellSize = par("zCellSize");

    maxTxPower = 0.0;

}               // readIniParameters

/* Parsing of custom pathloss map from a file,
 * filename given by the parameter pathLossMapFile
 */
void WirelessChannel::parsePathLossMap(void)
{
    if (strlen(pathLossMapFile) == 0)
        return;
    ifstream f(pathLossMapFile);
    if (!f.is_open())
        throw cRuntimeError("\n[Wireless Channel]:\n Error reading from pathLossMapFile %s\n", pathLossMapFile);

    string s;
    const char *ct;
    int source, destination;
    float pathloss_db;

    /* each line in a file is in the same format:
     * Transmitter_id>Receiver_id:PathLoss_dB, ... ,Receiver_id:PathLoss_dB
     * based on these values we will update the pathloss array
     * (using updatePathLossElement function)
     */
    while (getline(f, s)) {
        ct = s.c_str(); //ct points to the beginning of a line
        while (ct[0] && (ct[0] == ' ' || ct[0] == '\t'))
            ct++;
        if (!ct[0] || ct[0] == '#')
            continue;   // skip comments
        if (parseInt(ct, &source))
            throw cRuntimeError("\n[Wireless Channel]:\n Bad syntax in pathLossMapFile, expecting source identifier\n");
        while (ct[0] && ct[0] != '>')
            ct++;   //skip untill '>' character
        if (!ct[0])
            throw cRuntimeError("\n[Wireless Channel]:\n Bad syntax in pathLossMapFile, expecting comma separated list of values\n");
        cStringTokenizer t(++ct, ",");  //divide the rest of the strig with comma delimiter
        while ((ct = t.nextToken())) {
            if (parseInt(ct, &destination))
                throw cRuntimeError("\n[Wireless Channel]:\n Bad syntax in pathLossMapFile, expecting target identifier\n");
            while (ct[0] && ct[0] != ':')
                ct++;   //skip untill ':' character
            if (parseFloat(++ct, &pathloss_db))
                throw cRuntimeError("\n[Wireless Channel]:\n Bad syntax in pathLossMapFile, expecting dB value for path loss\n");
            updatePathLossElement(source, destination, pathloss_db);
        }
    }
}

//This function will update a pathloss element for given source and destination cells with a given value of pathloss
//If this pair is already defined in pathloss array, the old value is replaced, otherwise a new pathloss element is created
void WirelessChannel::updatePathLossElement(int src, int dst, float pathloss_db)
{
    if (src >= numOfSpaceCells || dst >= numOfSpaceCells) return;
    list <PathLossElement*>::iterator it1;
    for (it1 = pathLoss[src].begin(); it1 != pathLoss[src].end(); it1++) {
        if ((*it1)->cellID == dst) {
            (*it1)->avgPathLoss = pathloss_db;
            return;
        }
    }
    pathLoss[src].push_front(new PathLossElement(dst, pathloss_db));
}

//wrapper function for atoi(...) call. returns 1 on error, 0 on success
int WirelessChannel::parseInt(const char *c, int *dst)
{
    while (c[0] && (c[0] == ' ' || c[0] == '\t'))
        c++;
    if (!c[0] || c[0] < '0' || c[0] > '9')
        return 1;
    *dst = atoi(c);
    return 0;
}

//wrapper function for strtof(...) call. returns 1 on error, 0 on success
int WirelessChannel::parseFloat(const char *c, float *dst)
{
    char *tmp;
    *dst = strtof(c, &tmp);
    if (c == tmp)
        return 1;
    return 0;
}

float WirelessChannel::calcpathloss(double distance, double sand, double clay, double freq, double moisture, double Ptransmit )
{
    // Lateral waves based path loss model for WUSN

    double pb=1.5;
    double ps=2.663;
    double a1=0.65;
    double u=12.57e-7;
    double a_const=0.58e-12;
    double ew_0=80.1;
    double ew_inf =4.9;
    double e_0 = 8.85e-12;

    double theta_r=3.1416/4;
    double h_t=0.4;
    double h_r=0.5;

    double r_1;
    double r_2;
    double es;
    double ef1;
    double ef2;
    double b1;
    double b2;
    double ref_index;
    double num1;
    double num2;
    double Reflect;
    double T;
    double conduc_eff;
    double e1;
    double e2;
    double alpha_s;
    double beta_s;
    double lemda_s;

    double P_direct;
    double P_reflect;
    double P_lateral;
    double P_total;
    double u_clay;

    r_1 = sqrt(pow((h_t-h_r),2) + pow(distance,2));
    r_2 = sqrt(pow((h_t+h_r),2) + pow(distance,2));

    es =  pow((1/((pb/ps)+1)),2);

    u_clay= 1-clay;

    conduc_eff = 0.0467 + (0.2204 * pb) - (0.4111 * sand) + (0.6614 * u_clay);

    ef1 = ew_inf + ((ew_0 - ew_inf)/(1+pow((a_const*freq*1e6),2)));
    ef2 = ((a_const*freq*1e6*(ew_0 - ew_inf))/(1+pow((a_const*freq*1e6),2)))+((conduc_eff*(ps-pb))/(2*3.1416*e_0*ps*moisture*freq*1e6));

    b1=1.2748-(0.519*sand)-(0.152*u_clay);
    b2=1.33797-(0.603*sand)-(0.166*u_clay);

    e1=1.15*(pow((1+((pb/ps)*(pow(es,(a1))))+((pow(moisture,b1))*(pow(ef1,a1)))-moisture),(1./a1)))-0.68;
    e2=pow((pow(moisture,b2)*pow(ef2,a1)),(1/a1));

    ref_index = sqrt((sqrt(pow(e1,2) + pow(e2,2)) + e1 )/2);

    num1=(h_t + h_r)*(1/(r_2));
    num2=sqrt(1-pow((sin(theta_r)),2));

    Reflect = (((1/ref_index)*num1) - num2)/(((1/ref_index)*num1) + num2);

    T =  (2*num1)/((ref_index*num1)+cos(3.1416/12));

    alpha_s =   2*3.1416*freq*(sqrt((u*e1*((sqrt(1+(pow((e2/e1),2))))-1))/2));
    beta_s =   2*3.1416*freq*(sqrt((u*e1*((sqrt(1+(pow((e2/e1),2))))+1))/2));

    lemda_s = (2*3.1416)/beta_s ;

    P_direct = Ptransmit + 20*log10(lemda_s) - 20*log10(r_1)- (8.69*alpha_s*r_1) - 45 ;

    P_reflect = Ptransmit + 20*log10(lemda_s) - 20*log10(r_2)- (8.69*alpha_s*r_2) + 20*log10(abs(Reflect))-45;

    P_lateral = Ptransmit + 20*log10(lemda_s) - 40*log10(distance) - ((8.69*alpha_s)*(h_t+h_r)) + 20*log10(T) -30;

    P_total = -10*log10(pow(10,(P_direct/10)) + pow(10,(P_reflect/10)) + pow(10,(P_lateral/10))) ;

    return (P_total);

}

double WirelessChannel::randnum (double a, double b)
{
    static std::default_random_engine generator;
    std::uniform_real_distribution<double> distribution (a,b);
    return distribution(generator);
}
