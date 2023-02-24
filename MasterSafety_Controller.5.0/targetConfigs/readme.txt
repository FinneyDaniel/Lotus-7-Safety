 
/******************************/
30-06-22
safety + basic CAN module added. state machine , scheduler added.
////////////////////////////////////////////////////////////////////
bootloader tested. CAN id changed as per the CAN matrix sheet

PENDING : 
1.check safety module . watch dog enable is problem. if enable the dog its getting reset continuously . need to be checked.
2.automatic state transition code to be added and checked.
3.add the communication failure flags of iocard and MP & MS in statemachine.
4.function timings to be measured for prepare the document.
5.Fault = DOS Not OK--  masked in faultcheck.c
6.Critical Fault = Purge Fans Not OK |EStop from System Safety Controller --  masked in faultcheck.c
faultcheck.c -> Fault , SHUT_down states are masked. for testing purpose.

7.safety_cancom MASKED in safety_init.c
mp------

When i need to be set the STOP command to MP
send fault and digital op status to MP
doubts:
MS is ARMED power state , if stop cmd received from SS what is the off delay of PRUGE fans. Armed to STANDBY state.
/****************************************************************************************/
1.	On loss of communication from LTC-CB (System Controller) for a period of 30 seconds, the Master Safety Controller shall enter RA state within a period of 25 seconds.
2.	The Master Controller shall purge the LHC as follows:
a.	Turn on FAN-401/402/501/502/101/102
b.	Monitor the tach feedback from FAN-401/402/501/502/101/102
c.	The tach feedback should be more than 3000 RPM for a period of 900 seconds.
d.	If the above conditions are not met after the 900 second period, the Master Safety Controller shall enter RA state within a period of 25 seconds.
3.	When Hydrogen is being generated:
a.	On detection of fan tach feedback failure (less than 600 RPM) in any of FAN-401/402/501/502/101/102, the Master Safety Controller shall enter RA state within a period of 25 seconds.
b.	On detection of DSW-101/401/501 fault, the Master Safety Controller shall enter RA state within a period of 25 seconds.
c.	If PRT-102 reads less than 0.5 bar, then the Master Safety Controller shall enter RA state within a period of 25 seconds.
d.	If LVL-101 reads less than 60% then the Master Safety Controller shall enter RA state within a period of 25 seconds.
e.	If PRT-401 reads less than 0.01 times the current set point, then the Master safety Controller shall enter RA state within a period of 25 seconds.  This check is started only after 100 seconds after current set point is set.
4.	On loss of communication from IO Board for a period of 30 seconds, the Master Controller shall enter RA state within a period of 25 seconds.
5.	On reception of hydrogen stop command from System Safety Controller, the Master Safety Controller shall de-pressurize the dryer as follows:
a.	Keep FAN-101/102/401/402/501/502 ON.
b.	Open valve SV 7
c.	Wait for 5 seconds
d.	Open valve SV 3/4/5/6
/***********************************correction to be done *************************************/
11-08-22

delay to be changed in fault_chk.c -> like purge FAn fault set and clear delay . 

state_machine.c -> state conditions are need to be changed. presently conditions are masked.

canaCom.c -> CANA_fnTx(). cases are masked. for manual testing purpose

/*********************** LPC control box testing at cKB *********************************/

29-08-22

control_loop.c -> as to be verified (PSU contactor digital output is getting high/low. that should be high when IO Power state)

/--------------------------------------------------------------------------------------/
07-11-22
faultcheck.c -> purgefan related conditions are changed. 
canaCom.c -> commstart for LPC LHC MP related bits are removed.
Testing at CKB - basic communication between MP & SP and SS<->MS done.
/*************************************PURGE FAN SPEED****in all STATE except STANDBY*******************************/
//    if ((STAT_tzStateMac.Present_st > 0)
//            && ((CANA_tzMPRegs.Hyd_Genstart == 0)
//                    && (CANA_tzMPRegs.PresentStMP < 3)))
    if ((STAT_tzStateMac.Present_st >=3)
             && ((CANA_tzMPRegs.PresentStMP < 3)))
    {
        if ((CANA_tzDISensorData.PURGE101 < uiTachFreqHighLmt)
                || (CANA_tzDISensorData.PURGE102 < uiTachFreqHighLmt))

        {
            CANA_tzLPCIO1_AOFaultRegs.bit.PUR_FAN101_102 = faultLPCIO1_fnSet(
                    FAULT_mPURFAN101_102, DLY_mFLTSET_PF);
        }

        else
        {
            CANA_tzLPCIO1_AOFaultRegs.bit.PUR_FAN101_102 = 0;
            faultLPCIO1_fnReset(
            FAULT_mPURFAN101_102,
                                10);
        }
    }
    /*'''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''*/
    if ((STAT_tzStateMac.Present_st >=3)
            && ((CANA_tzMPRegs.PresentStMP < 3)))
    {
        if ((CANA_tzDISensorData.PURGE501 < uiTachFreqHighLmt)
                    || (CANA_tzDISensorData.PURGE502 < uiTachFreqHighLmt))

        {
            CANA_tzLPCIO1_AOFaultRegs.bit.PUR_FAN501_502 = faultLPCIO1_fnSet(
                    FAULT_mPURFAN501_502, DLY_mFLTSET_PF);
        }
        else
        {
            CANA_tzLPCIO1_AOFaultRegs.bit.PUR_FAN501_502 = 0;
            faultLPCIO1_fnReset(
            FAULT_mPURFAN501_502,
                                10);
        }
    }
    /*'''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''*/
  //   if((STAT_tzStateMac.Present_st > 0)&&((CANA_tzMPRegs.Hyd_Genstart==0)&&(CANA_tzMPRegs.PresentStMP<3)))
    if ((STAT_tzStateMac.Present_st >= 3) && ((CANA_tzMPRegs.PresentStMP < 3)))

    {
        if ((CANA_tzDISensorData.PURGE401 < uiTachFreqHighLmt)
                || (CANA_tzDISensorData.PURGE402 < uiTachFreqHighLmt))
        {
            CANA_tzLPCIO1_AOFaultRegs.bit.PUR_FAN401_402 = faultLPCIO1_fnSet(
                    FAULT_mPURFAN401_402, DLY_mFLTSET_PF);
        }

        else
        {
            CANA_tzLPCIO1_AOFaultRegs.bit.PUR_FAN401_402 = 0;
            faultLPCIO1_fnReset(
            FAULT_mPURFAN401_402,
                                10);
        }
    }
/****************** fault check  when Hydrogen generated condition ************************************/
    if (CANA_tzMPRegs.PresentStMP >= 3)
    {
        if ((STAT_tzStateMac.Present_st > 0)
                && ((CANA_tzMPRegs.Hyd_Genstart == 0)))
        {
            if ((CANA_tzDISensorData.PURGE101 < uiTachFreqLowLmt)
                    || (CANA_tzDISensorData.PURGE102 < uiTachFreqLowLmt))
            {
                CANA_tzLPCIO1_AOFaultRegs.bit.PUR_FAN101_102 =
                        faultLPCIO1_fnSet(
                        FAULT_mPURFAN101_102,
                                          DLY_mFLTSET_PF1);
            }
            else
            {
                CANA_tzLPCIO1_AOFaultRegs.bit.PUR_FAN101_102 = 0;
                faultLPCIO1_fnReset(
                FAULT_mPURFAN101_102,
                                    10);
            }

            /*'''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''*/

            if ((STAT_tzStateMac.Present_st > 0)
                    && ((CANA_tzMPRegs.Hyd_Genstart == 0)))
            {
                if ((CANA_tzDISensorData.PURGE501 < uiTachFreqLowLmt)
                        || (CANA_tzDISensorData.PURGE502 < uiTachFreqLowLmt))
                {
                    CANA_tzLPCIO1_AOFaultRegs.bit.PUR_FAN501_502 =
                            faultLPCIO1_fnSet(
                            FAULT_mPURFAN501_502,
                                              DLY_mFLTSET_PF1);
                }
                else
                {
                    CANA_tzLPCIO1_AOFaultRegs.bit.PUR_FAN501_502 = 0;
                    CANA_tzLPCIO1_AOFaultRegs.bit.PUR_FAN501_502 =
                            faultLPCIO1_fnReset(
                            FAULT_mPURFAN501_502,
                                                DLY_mFLTCLEAR_PF1);
                }
            }
            /*'''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''*/

            if ((STAT_tzStateMac.Present_st > 0)
                    && ((CANA_tzMPRegs.Hyd_Genstart == 0)))
            {
                if ((CANA_tzDISensorData.PURGE401 < uiTachFreqLowLmt)
                        || (CANA_tzDISensorData.PURGE402 < uiTachFreqLowLmt))
                {
                    CANA_tzLPCIO1_AOFaultRegs.bit.PUR_FAN401_402 =
                            faultLPCIO1_fnSet(
                            FAULT_mPURFAN401_402,
                                              DLY_mFLTSET_PF1);
                }

                else
                {
                    CANA_tzLPCIO1_AOFaultRegs.bit.PUR_FAN401_402 = 0;
                    CANA_tzLPCIO1_AOFaultRegs.bit.PUR_FAN401_402 =
                            faultLPCIO1_fnReset(
                            FAULT_mPURFAN401_402,
                                                DLY_mFLTCLEAR_PF1);
                }
            }

        }

