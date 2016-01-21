%%%%%%%%%%%%%%���ݸ�ͨ������ʱ���spike������ͨ��ѵ���õ��Ļع�ģ�ͼ�����Ӧ���˶���%%%%%%%%%%%%%%%%
function vecMov =ImplElec_Test(matSpikeCount, strFileName)
%   ����˵��
%	matSpikeCount���ξ���, �����ź�ͨ��ÿʱ���spike����. ����ΪNumNeuChan*NumTimeBin (���ź�ͨ����*ʱ�����)
%   NumTimeBin������ڵ���ģ���ļ��е�nEpo
%	vecMov��������,�������źźͻع�ģ��Ԥ��Ķ���. ����ΪNumEqChan (�˶���֪�豸ͨ����)
%   Ŀǰҡ�˲���������,vecMov���ֵ����ͨ����Ӧ�ķ���Ϊҡ�˷���
%	Ŀǰ��Ϊ������4��ͨ��. δ����Ϊ0, ����Ϊ1. ÿͨ��һ������,�����Ժ������豸��չ
%	strFileName���洢ģ�͵��ļ���, ��չ��������".mat"
%   nWorkMode ����ģʽ, ��ģ���ļ��ж�ȡ. 1Ϊ����ģ��; 2����(����)����ģ��

%   Initializing and checking
load(strFileName);
%   variables included 'matC','nWorkMode','NumNeuChan','NumEqChan','nEpo'
if(size(matSpikeCount,2)~=nEpo)
    error('The length of signal does not match nEpo.');
end
if(min(min(matSpikeCount))<0)
    error('Negative value exists in matSpikeCount');
end

switch nWorkMode
    case 1
        matSig=matSpikeCount;
    case 2
        matSig=log(matSpikeCount+1);
    otherwise
        error('Undefined work mode');
end

%Galculate the movement
if( nWorkMode==1 ||nWorkMode==2)
    vecX=zeros(NumNeuChan*nEpo,1);
    for indEpo=1:nEpo
        vecX((indEpo-1)*NumNeuChan+1:indEpo*NumNeuChan,1)=matSig(:,1+nEpo-indEpo);
    end
    vecMov= matC*vecX; 
end


end