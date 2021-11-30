#include <cstddef>
#include <shlwapi.h>
#include <string>

struct MY_PROCESS_INFO//��������� � ������� ����� ��������� ���� � ��������
{
	PROCESS_INFORMATION procInfo;//������ id � handle �������� � ��� ������
	bool state;//������� ������� ��� ��������
	std::string name;//���� ���������� �����
	LPTSTR result;//��������� ������ ��������
};


template <typename dataType>
class Node//������� ������
{
public:
	dataType data;
	Node<dataType>* next;
	Node(dataType d) :data(d)
	{
		next = NULL;
	}
};

template <typename dataType>
class ProcessList//������� ����������� ������, �������� ��������
{
public:
	Node<dataType>* first;
	int size;
	ProcessList() :first(NULL), size(0) {}
	void AddNode(dataType newData)
	{
		Node<dataType>* newNode = new Node<dataType>(newData);
		if (first == NULL)
		{
			first = newNode;
		}
		else
		{
			Node<dataType>* currentNode = first;
			while (currentNode->next != NULL)
			{
				currentNode = currentNode->next;
			}
			currentNode->next = newNode;
		}
		size++;
	}
	MY_PROCESS_INFO GetInfo(int needNubmer)
	{
		Node<dataType>* currentNode = first;
		for (int i = 0;currentNode != NULL;i++)
		{
			if (i == needNubmer)
			{
				return currentNode->data;
			}
			currentNode = currentNode->next;
		}
		throw "GetInfo error";
	}
	BOOL GetProcessInfo(PROCESS_INFORMATION* bufferInfo,int needNubmer)//����� ������ �������
	{
		Node<dataType>* currentNode = first;
		for (int i = 0;currentNode != NULL;i++)
		{
			if (i == needNubmer)
			{
				currentNode->data.state = FALSE;
				*bufferInfo = currentNode->data.procInfo;
				return TRUE;
			}
			currentNode = currentNode->next;
		}
		return FALSE;
	}
	void Check()//�������� ���������� ���������
	{
		Node<dataType>* currentNode = first;
		DWORD dwWaitResult;
		while (currentNode != NULL)
		{
			if (currentNode->data.state)
			{
				dwWaitResult = WaitForSingleObject(
					currentNode->data.procInfo.hProcess,
					5);
				switch (dwWaitResult)
				{
				case WAIT_OBJECT_0://The state of the specified object is signaled. 
					currentNode->data.state = FALSE;
					break;
				case WAIT_TIMEOUT://The time-out interval elapsed, and the object's state is nonsignaled. 
					
					break;
				case WAIT_FAILED://The function has failed. To get extended error information, call GetLastError. 
					currentNode->data.state = FALSE;
					break;
				case WAIT_ABANDONED:
					/*The specified object is a mutex object that was not released by the thread that owned the mutex object
					before the owning thread terminated.Ownership of the mutex object is granted to the calling threadand the
					mutex state is set to nonsignaled.
					If the mutex was protecting persistent state information, you should check it for consistency.*/
					currentNode->data.state = FALSE;
					break;
				}
			}
			currentNode = currentNode->next;
		}
	}
	void PrintLast(HWND hEditWnd)//���������� ���� � ������ce
	{
		MY_PROCESS_INFO lastInfo = GetInfo(size - 1);
		AppendText(hEditWnd, std::wstring(lastInfo.name.begin(), lastInfo.name.end()));
		AppendText(hEditWnd, L":\r\n");
		AppendText(hEditWnd, lastInfo.result);
	}
	void AppendText(HWND hEditWnd, LPTSTR Text)//������� ����� � ����
	{
		int idx = GetWindowTextLength(hEditWnd);
		SendMessage(hEditWnd, EM_SETSEL, (WPARAM)idx, (LPARAM)idx);
		SendMessage(hEditWnd, EM_REPLACESEL, 0, (LPARAM)Text);
	}
private:
	void AppendText(HWND hEditWnd, std::wstring str)//������� ����� � ����
	{
		std::basic_string<TCHAR> converted(str.begin(), str.end());
		LPCTSTR Text = converted.c_str();
		int idx = GetWindowTextLength(hEditWnd);
		SendMessage(hEditWnd, EM_SETSEL, (WPARAM)idx, (LPARAM)idx);
		SendMessage(hEditWnd, EM_REPLACESEL, 0, (LPARAM)Text);
	}
};

