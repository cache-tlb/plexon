%%%%%%%%%%%%%%���ݸ�ͨ��ÿʱ���spike��������Ӧ���˶��������ع�ģ��%%%%%%%%%%%%%%%%
function fErr =ImplElec_Train(matSpikeCount,matMov,nEpo,strFileName,nWorkMode)
%   ����˵��
%	matSpikeCount���ξ���, �����ź�ͨ��ÿʱ���spike����. ����ΪNumNeuChan*NumTimeBin (���ź�ͨ����*ʱ�����)
%	matMov���ξ���,ÿʱ��δ��˶���֪�豸����ȡ�Ķ���. ����ΪNumEqChan*NumTimeBin (�˶���֪�豸ͨ����*ʱ�����)
%	nEpo������ϵ��ź�ʱ�����. ���ӵ�ǰʱ���nTNow��nTNow-nEpo+1ʱ���
%	Ŀǰ��Ϊ������5��ͨ��. �ֱ�Ϊ�ϡ��¡����ҡ�ͣ. δ����Ϊ0, ����Ϊ1. ÿͨ��һ������,�����Ժ������豸��չ
%	strFileName���洢ģ�͵��ļ���, ��չ��������".mat"
%   nWorkMode ����ģʽ. 1Ϊ����ģ��; 2����(����)����ģ��

%   Initializing and checking
[NumNeuChan,NumTimeBin]= size(matSpikeCount);
[NumEqChan,NumTimeBinTmp]= size(matMov);

if(NumTimeBin~=NumTimeBinTmp)
    error('The length of signal and movments do not match.');
end
if(NumEqChan*NumTimeBin<nEpo*NumNeuChan)
    error('NumEqChan*NumTimeBin<nEpo*NumNeuChan, not enough information to fit.');
end
if(min(min(matSpikeCount))<0)
    error('Negative value exists in matSpikeCount');
end
if(max(max(matMov))>1 || min(min(matMov))<0)
    error('Matcov should be between 0 and 1');
end

switch nWorkMode
    case 1
        matSig=matSpikeCount;
    case 2
        matSig=log(matSpikeCount+1);
    otherwise
        error('Undefined work mode');
end


%Galculate the model
if( nWorkMode==1 ||nWorkMode==2)
    matX=zeros(NumNeuChan*nEpo,NumTimeBin-nEpo+1);
    matY=matMov(:,nEpo:end);
    for indData=1:NumTimeBin-nEpo+1
        for indEpo=1:nEpo
            matX((indEpo-1)*NumNeuChan+1:indEpo*NumNeuChan,indData)=matSig(:,indData+nEpo-indEpo);
        end
    end
    
    %Calculate the coefficients
    matC= matY*matX'/(matX*matX');
    %Calculate the residual
    matQ= matY-matC*matX;    
end
    

%Save the model file

save(strFileName,'matC','nWorkMode','NumNeuChan','NumEqChan','nEpo');
fErr= mean(mean(matQ.^2));
end