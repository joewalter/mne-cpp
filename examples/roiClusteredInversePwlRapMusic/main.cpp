//=============================================================================================================
/**
* @file     main.cpp
* @author   Christoph Dinh <chdinh@nmr.mgh.harvard.edu>
* @version  1.0
* @date     March, 2014
*
* @section  LICENSE
*
* Copyright (C) 2014, Christoph Dinh. All rights reserved.
*
* Redistribution and use in source and binary forms, with or without modification, are permitted provided that
* the following conditions are met:
*     * Redistributions of source code must retain the above copyright notice, this list of conditions and the
*       following disclaimer.
*     * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and
*       the following disclaimer in the documentation and/or other materials provided with the distribution.
*     * Neither the name of MNE-CPP authors nor the names of its contributors may be used
*       to endorse or promote products derived from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
* WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
* PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
* INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
* PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
* HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
* NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
* POSSIBILITY OF SUCH DAMAGE.
*
*
* @brief    Example of the computation of a roi clustered inverse rap music
*
*/


//*************************************************************************************************************
//=============================================================================================================
// INCLUDES
//=============================================================================================================

#include <fs/label.h>
#include <fs/surfaceset.h>
#include <fs/annotationset.h>

#include <fiff/fiff_evoked.h>
#include <fiff/fiff.h>
#include <mne/mne.h>

#include <mne/mne_sourceestimate.h>
#include <inverse/rapMusic/pwlrapmusic.h>

#include <disp3D/view3D.h>
#include <disp3D/control/control3dwidget.h>

#include <utils/mnemath.h>

#include <iostream>


//*************************************************************************************************************
//=============================================================================================================
// QT INCLUDES
//=============================================================================================================

#include <QApplication>
#include <QSet>


//*************************************************************************************************************
//=============================================================================================================
// USED NAMESPACES
//=============================================================================================================

using namespace MNELIB;
using namespace FSLIB;
using namespace FIFFLIB;
using namespace INVERSELIB;
using namespace DISP3DLIB;
using namespace UTILSLIB;


//*************************************************************************************************************
//=============================================================================================================
// MAIN
//=============================================================================================================

//=============================================================================================================
/**
* The function main marks the entry point of the program.
* By default, main has the storage class extern.
*
* @param [in] argc (argument count) is an integer that indicates how many arguments were entered on the command line when the program was started.
* @param [in] argv (argument vector) is an array of pointers to arrays of character objects. The array objects are null-terminated strings, representing the arguments that were entered on the command line when the program was started.
* @return the value that was set to exit() (which is 0 if exit() is called via quit()).
*/
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QFile t_fileRaw("./MNE-sample-data/MEG/sample/sample_audvis_raw.fif");
    QFile t_fileFwd("./MNE-sample-data/MEG/sample/sample_audvis-eeg-oct-6-fwd.fif");
    AnnotationSet t_annotationSet("./MNE-sample-data/subjects/sample/label/lh.aparc.a2009s.annot", "./MNE-sample-data/subjects/sample/label/rh.aparc.a2009s.annot");
    SurfaceSet t_surfSet("./MNE-sample-data/subjects/sample/surf/lh.white", "./MNE-sample-data/subjects/sample/surf/rh.white");

    QString t_sFileNameStc("sample_audvis_raw_RAP_MUSIC.stc");

    qint32 numDipolePairs = 7;

    qint32 samplesStcWindow = 300;
    float stcOverlap = 0.0f;

    qint32 startSample = 0;
    qint32 numSample = 40000;

    bool in_samples = true;
    bool keep_comp = true;

    // Parse command line parameters
    for(qint32 i = 0; i < argc; ++i)
    {
        if(strcmp(argv[i], "-stc") == 0 || strcmp(argv[i], "--stc") == 0)
        {
            if(i + 1 < argc)
                t_sFileNameStc = QString::fromUtf8(argv[i+1]);
        }
    }

    //
    // Read raw data
    //
    MNEForwardSolution t_Fwd(t_fileFwd);
    if(t_Fwd.isEmpty())
        return 1;

    QList<Label> t_qListLabels;
    QList<RowVector4i> t_qListRGBAs;

    //ToDo overload toLabels using instead of t_surfSet rr of MNESourceSpace
    t_annotationSet.toLabels(t_surfSet, t_qListLabels, t_qListRGBAs);

    QList<Label> t_qListLabelSelection;

    //LH
    t_qListLabelSelection << t_qListLabels[28] << t_qListLabels[29] << t_qListLabels[45] << t_qListLabels[66];
    //RH
    t_qListLabelSelection << t_qListLabels[103] << t_qListLabels[104] << t_qListLabels[120]<< t_qListLabels[141];

//    std::cout << "t_qListLabels.size " << t_qListLabels.size() << std::endl;

//    for(qint32 i = 0; i < t_qListLabels.size(); ++i)
//    {
//        qDebug() << "Num" << i << t_qListLabels[i].name;
//        std::cout << t_qListLabels[i].hemi << std::endl;
//    }

    MNEForwardSolution t_SelectFwd = t_Fwd.pick_regions(t_qListLabelSelection);

    //
    //   Setup for reading the raw data
    //
    FiffRawData raw(t_fileRaw);

    float from = raw.first_samp + startSample;
    float to = from + numSample - 1;

    //
    //   Set up pick list: MEG + STI 014 - bad channels
    //
    QStringList include;
//    include << "STI 014";
    bool want_meg   = false;
    bool want_eeg   = true;
    bool want_stim  = false;

    RowVectorXi picks = raw.info.pick_types(want_meg, want_eeg, want_stim, include);//, raw.info.bads);

    //
    //   Set up projection
    //
    qint32 k = 0;
    if (raw.info.projs.size() == 0)
        printf("No projector specified for these data\n");
    else
    {
        //
        //   Activate the projection items
        //
        for (k = 0; k < raw.info.projs.size(); ++k)
            raw.info.projs[k].active = true;

        printf("%d projection items activated\n",raw.info.projs.size());
        //
        //   Create the projector
        //
        fiff_int_t nproj = raw.info.make_projector(raw.proj);

        if (nproj == 0)
            printf("The projection vectors do not apply to these channels\n");
        else
            printf("Created an SSP operator (subspace dimension = %d)\n",nproj);
    }

    //
    //   Set up the CTF compensator
    //
    qint32 current_comp = raw.info.get_current_comp();
    qint32 dest_comp = -1;

    if (current_comp > 0)
        printf("Current compensation grade : %d\n",current_comp);

    if (keep_comp)
        dest_comp = current_comp;

    if (current_comp != dest_comp)
    {
        qDebug() << "This part needs to be debugged";
        if(MNE::make_compensator(raw.info, current_comp, dest_comp, raw.comp))
        {
            raw.info.set_current_comp(dest_comp);
            printf("Appropriate compensator added to change to grade %d.\n",dest_comp);
        }
        else
        {
            printf("Could not make the compensator\n");
            return -1;
        }
    }
    //
    //   Read a data segment
    //   times output argument is optional
    //
    bool readSuccessful = false;
    MatrixXd data;
    MatrixXd times;
    if (in_samples)
        readSuccessful = raw.read_raw_segment(data, times, (qint32)from, (qint32)to, picks);
    else
        readSuccessful = raw.read_raw_segment_times(data, times, from, to, picks);

    if (!readSuccessful)
    {
        printf("Could not read raw segment.\n");
        return -1;
    }

    printf("Read %d samples.\n",(qint32)data.cols());

    //########################################################################################
    // RAP MUSIC Source Estimate

    //
    // Cluster forward solution;
    //
    MNEForwardSolution t_clusteredFwd = t_SelectFwd.cluster_forward_solution(t_annotationSet, 20);//t_Fwd.cluster_forward_solution_ccr(t_annotationSet, 20);//40);


    //
    // Compute inverse solution
    //
    PwlRapMusic t_pwlRapMusic(t_clusteredFwd, false, numDipolePairs);

    MNESourceEstimate sourceEstimate;

    float tstep = 1.0f/raw.info.sfreq;

    float tmin;
    if(in_samples)
        tmin = from * tstep;
    else
        tmin = from;

    //
    // Rap MUSIC Source estimate
    //
    sourceEstimate.data = MatrixXd::Zero(t_clusteredFwd.nsource, data.cols());

    //Results
    sourceEstimate.vertices = VectorXi(t_clusteredFwd.src[0].vertno.size() + t_clusteredFwd.src[1].vertno.size());
    sourceEstimate.vertices << t_clusteredFwd.src[0].vertno, t_clusteredFwd.src[1].vertno;

    sourceEstimate.times = RowVectorXf::Zero(data.cols());
    sourceEstimate.times[0] = tmin;
    for(qint32 i = 1; i < sourceEstimate.times.size(); ++i)
        sourceEstimate.times[i] = sourceEstimate.times[i-1] + tstep;
    sourceEstimate.tmin = tmin;
    sourceEstimate.tstep = tstep;


    bool first = true;
    bool last = false;

    qint32 t_iNumSensors = data.rows();
    qint32 t_iNumSteps = data.cols();

    qint32 t_iSamplesOverlap = (qint32)floor(((float)samplesStcWindow)*stcOverlap);
    qint32 t_iSamplesDiscard = t_iSamplesOverlap/2;

    MatrixXd measData = MatrixXd::Zero(t_iNumSensors, samplesStcWindow);

    qint32 curSample = 0;
    qint32 curResultSample = 0;
    qint32 stcWindowSize = samplesStcWindow - 2*t_iSamplesDiscard;

    while(!last)
    {
        //Data
        if(curSample + samplesStcWindow >= t_iNumSteps) //last
        {
            last = true;
            measData = data.block(0, data.cols()-samplesStcWindow, t_iNumSensors, samplesStcWindow);
        }
        else
            measData = data.block(0, curSample, t_iNumSensors, samplesStcWindow);


        curSample += (samplesStcWindow - t_iSamplesOverlap);
        if(first)
            curSample -= t_iSamplesDiscard; //shift on start t_iSamplesDiscard backwards

        //Calculate
        MNESourceEstimate stcData = t_pwlRapMusic.calculateInverse(measData, 0.0f, tstep);

        //Assign Result
        if(last)
            stcWindowSize = stcData.data.cols() - curResultSample;

        sourceEstimate.data.block(0,curResultSample,sourceEstimate.data.rows(),stcWindowSize) =
                                            stcData.data.block(0,0,stcData.data.rows(),stcWindowSize);

        curResultSample += stcWindowSize;

        if(first)
            first = false;
    }

    if(sourceEstimate.isEmpty())
        return 1;

    // Write stc to file
    if(!t_sFileNameStc.isEmpty())
    {
        QFile t_fileClusteredStc(t_sFileNameStc);
        sourceEstimate.write(t_fileClusteredStc);
    }

    //Source Estimate end
    //########################################################################################

    View3D::SPtr testWindow = View3D::SPtr(new View3D());
    testWindow->addSurfaceSet("Subject01", "HemiLRSet", t_surfSet, t_annotationSet);

    QList<BrainRTSourceLocDataTreeItem*> rtItemList = testWindow->addSourceData("Subject01", "HemiLRSet", sourceEstimate, t_clusteredFwd);

    testWindow->show();

    Control3DWidget::SPtr control3DWidget = Control3DWidget::SPtr(new Control3DWidget());
    control3DWidget->setView3D(testWindow);
    control3DWidget->show();

    if(!t_sFileNameStc.isEmpty())
    {
        QFile t_fileClusteredStc(t_sFileNameStc);
        sourceEstimate.write(t_fileClusteredStc);
    }

    return a.exec();
}
