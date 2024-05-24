#include <iostream>
#include <vector>
#include <random>
#include <cmath>
#include <fstream>


// OBJETOS PARA GERACAO DE NUMEROS ALEATORIOS
std::random_device gerador_semente; // cria o objeto de geracao de semente
int semente = gerador_semente(); // gera 1 semente
std::mt19937 gerador_aleatorios(semente); // constroi 1 objeto RNG
std::uniform_real_distribution<double> uniforme(0.0, 1.0); // objeto que gera dist unif



// FUNCOES

// Roda uma vez o loop com M passos de Monte Carlo e retorna os resultados
std::vector<std::vector<double>> loop_MC(const int N, double C_A, double C_B, const int M) {
    // parametros:
    // N    numero de celulas em cada compartimento
    // C_A  concentracao inicial de particulas no compartimento A
    // C_B  idem compartimento B
    // M    numero de passos de Monte Carlo

    // Periodicidade de coleta de dados
    const int x = M/100;

    // Obter numero inteiro de particulas em cada compartimento (truncado):
    int L_A = C_A * N;
    int L_B = C_B * N;

    // Variaveis a serem utilizadas no loop:
    bool v_A, v_B; // indica se ha particula na vizinhanca da abertura
    double D_mu_0, D_mu_1, D_mu_2; // energia livre dos microestados sob comparacao
                                   // 0: microestado atual
                                   // 1: microestado se passa particula de A->B
                                   // 2: microestado se passa particula de B->A
    std::vector<double> iteracao = {1}; // indice da iteracao para coleta de dados
    std::vector<double> dados_C_A = {C_A}; // coleta de dados de C_A
    std::vector<double> dados_C_B = {C_B}; // coleta de dados de C_B

    // Loop principal
    for (int i = 0; i < M; i++) {
        // probabilidades de haver particula na vizinhanca
        if (uniforme(gerador_aleatorios) < C_A) {
            v_A = true;
            // para otimizacao, calculamos aqui a energia do microestado com passagem A->B:
            D_mu_1 = std::log( ((L_A - 1) / static_cast<double>(N)) / ((L_B + 1) / static_cast<double>(N)) );
        } else {
            v_A = false;
        }
        if (uniforme(gerador_aleatorios) < C_B) {
            v_B = true;
            // para otimizacao, calculamos aqui a energia do microestado com passagem B->A:
            D_mu_2 = std::log( ((L_A + 1) / static_cast<double>(N)) / ((L_B - 1) / static_cast<double>(N)) );
        } else {
            v_B = false;
        }

        // se nao houver nada na vizinhanca, pula p proxima iteracao do loop
        if (v_A + v_B == 0) {
            if ( (i + 1) % x == 0) { // antes, coleta os dados se o passo estiver na periodicidade
                iteracao.push_back(i + 1);
                dados_C_A.push_back(C_A);
                dados_C_B.push_back(C_B);
            }
            continue;
        }

        // energia do estado atual
        D_mu_0 = std::log(C_A/C_B);

        // passagem de particula A->B
        if (v_A) {
            if (D_mu_1 < D_mu_0) { 
                L_A--;
                L_B++;
            } else if (uniforme(gerador_aleatorios) < std::exp(D_mu_0 - D_mu_1)) {
                L_A--;
                L_B++;
            }
            // atualiza valor da concentracao
            C_A = L_A / static_cast<double>(N);
        }

        // passagem de particula B->A
        if (v_B) { 
            if (D_mu_2 < D_mu_0) { 
                L_A++;
                L_B--;
            } else if (uniforme(gerador_aleatorios) < std::exp(D_mu_0 - D_mu_2)) {
                L_A++;
                L_B--;
            }
            // atualiza valor da concentracao
            C_B = L_B / static_cast<double>(N);
        }

        // coleta de dados na periodicidade
        if ( (i + 1) % x == 0) {
            iteracao.push_back(i + 1);
            dados_C_A.push_back(C_A);
            dados_C_B.push_back(C_B);
        }
    }

    return {iteracao, dados_C_A, dados_C_B};

}

// Roda o loop de Monte Carlo varias vezes e retorna os resultados
std::vector<std::vector<double>> multiplos_loops_MC(
    const int n_loops, // numero de loops
    const int N, double C_A, double C_B, const int M // demais parametros pra funcao loop_MC
) {
    // barra de progresso
    std::cout << "Progresso:\n" << "0%       100%\n" << "|          |\n ";
    const int periodo_print = n_loops / 10;
    // loops
    std::vector<std::vector<double>> resultado = loop_MC(N, C_A, C_B, M);
    std::vector<std::vector<double>> novo;
    for (int i = 0; i < n_loops-1; i++) {
        novo = loop_MC(N, C_A, C_B, M);
        resultado.push_back(novo[1]);
        resultado.push_back(novo[2]);
        if (i % periodo_print == 0) {
            std::cout << '.'; // barra de progresso
        }
    }
    std::cout << '\n';
    return resultado;
}

// Calcula media dos resultados e retorna uma tabela
std::vector<std::vector<double>> consolidar_medias(std::vector<std::vector<double>> resultados_brutos) {
    // o argumento eh simplesmente algo que foi retornado da funcao multiplos_loops_MC

    // variaveis
    const int n_colunas = resultados_brutos.size() - 1;
    const int n_loops = n_colunas / 2;
    std::vector<double> media_C_A = {};
    std::vector<double> media_C_B = {};
    std::vector<double> delta = {};
    std::vector<double> desv_pad_C_A = {};
    std::vector<double> desv_pad_C_B = {};
    std::vector<double> erro_delta = {};
    
    // para cada linha de resultados (iteracao de mesmo indice nos varios loops):
    for (int i = 0; i < resultados_brutos[0].size(); i++) {    
        // variaveis
        double soma_C_A = 0;
        double soma_C_B = 0;
        double soma_desvios_C_A = 0;
        double soma_desvios_C_B = 0;
        // somar as concentracoes de cada compartimento
        for (int j = 1; j < n_colunas; j += 2) {
            soma_C_A += resultados_brutos[j][i];
            soma_C_B += resultados_brutos[j+1][i];
        }
        // calcular as medias e o delta
        double media_C_A_linha = soma_C_A / n_loops;
        double media_C_B_linha = soma_C_B / n_loops;
        double delta_linha = media_C_A_linha - media_C_B_linha;
        // calcular desvio padrao
        for (int j = 1; j < n_colunas; j += 2) {
            soma_desvios_C_A += std::pow(resultados_brutos[j][i] - media_C_A_linha,2);
            soma_desvios_C_B += std::pow(resultados_brutos[j+1][i] - media_C_B_linha,2);
        }
        const double desv_pad_C_A_linha = std::sqrt(soma_desvios_C_A/n_colunas);
        const double desv_pad_C_B_linha = std::sqrt(soma_desvios_C_B/n_colunas);
        // juntar esta linha a tabela a ser retornada
        media_C_A.push_back(media_C_A_linha);
        media_C_B.push_back(media_C_B_linha);
        delta.push_back(delta_linha);
        desv_pad_C_A.push_back(desv_pad_C_A_linha);
        desv_pad_C_B.push_back(desv_pad_C_B_linha);
        erro_delta.push_back(std::sqrt( std::pow(desv_pad_C_A_linha,2) + std::pow(desv_pad_C_B_linha,2)));
    }

    // resultado
    return {resultados_brutos[0], media_C_A, media_C_B, delta, desv_pad_C_A, desv_pad_C_B, erro_delta};
}

// Salva resultados brutos em formato csv
void resultados_para_csv(std::vector<std::vector<double>> dados, const std::string caminho) {
    // argumentos:
    // dados    resultado da funcao consolidar medias
    // caminho  caminho e nome do arquivo a ser salvo

    // cria objeto de representacao do arquivo
    std::ofstream arquivo(caminho);
    // adiciona cabecalho
    std::string cabecalho = "t";
    std::string cjto_dados = ",C_A,C_B";
    const int n_cjtos_dados = (dados.size() - 1) / 2;
    for (int i = 0; i < n_cjtos_dados; i++) {
        cabecalho += cjto_dados;
    }
    arquivo << cabecalho << '\n';
    // adiciona linhas da tabela
    const int n_colunas = dados.size();
    const int n_linhas = dados[0].size();
    for (int linha = 0; linha < n_linhas; linha++) {
        for (int coluna = 0; coluna < n_colunas; coluna++) {
            arquivo << dados[coluna][linha];
            // se nao eh o ultimo elemento da linha, add virgula
            if (coluna != n_colunas - 1) {
                arquivo << ",";
            }
        }
        arquivo << "\n";
    }
    arquivo.close();
}

// Salva medias em formato csv
void medias_para_csv(std::vector<std::vector<double>> dados, const std::string caminho) {
    // argumentos:
    // dados    resultado da funcao consolidar medias
    // caminho  caminho e nome do arquivo a ser salvo

    // cria objeto de representacao do arquivo
    std::ofstream arquivo(caminho);
    // adiciona cabecalho
    std::string cabecalho = "t,C_A,C_B,delta,desv_pad_C_A,desv_pad_C_B,erro_delta"; // add desvpad
    arquivo << cabecalho << '\n';
    // adiciona linhas da tabela
    const int n_colunas = dados.size();
    const int n_linhas = dados[0].size();
    for (int linha = 0; linha < n_linhas; linha++) {
        for (int coluna = 0; coluna < n_colunas; coluna++) {
            arquivo << dados[coluna][linha];
            // se nao eh o ultimo elemento da linha, add virgula
            if (coluna != n_colunas - 1) {
                arquivo << ",";
            }
        }
        arquivo << "\n";
    }
    arquivo.close();
}

// Modelo completo (todas as funcoes juntas)
void modelo_completo(
    const std::string caminho_resultados_brutos, // onde salvar dados brutos
    const std::string caminho_medias, // onde salvar dados das medias
    const int n_loops, const int N, double C_A, double C_B, const int M // parametros da funcao multiplos loops
) {
    std::vector<std::vector<double>> resultados_brutos = multiplos_loops_MC(n_loops, N, C_A, C_B, M);
    resultados_para_csv(resultados_brutos, caminho_resultados_brutos);
    std::vector<std::vector<double>> medias = consolidar_medias(resultados_brutos);
    medias_para_csv(medias, caminho_medias);
}


// PROGRAMA
int main()
{   
    // Configuracao 1: 
    // N = 1000; C_A = 0.5; C_B = 0.25
    // Apos testes, uso M = 10000
    std::cout << "Iniciada simulacao da configuracao 1\n";
    modelo_completo("resultados brutos/config-1.csv", "resultados consolidados (medias)/config-1.csv", 100, 1000, 0.5, 0.25, 10000);
    std::cout << "Concluida simulacao da configuracao 1\n\n";
    
    // Configuracao 2: 
    // N = 1000; C_A = 0.5; C_B = 0.005
    // Apos testes, uso M = 10000
    std::cout << "Iniciada simulacao da configuracao 2\n";
    modelo_completo("resultados brutos/config-2.csv", "resultados consolidados (medias)/config-2.csv", 100, 1000, 0.5, 0.005, 10000);
    std::cout << "Concluida simulacao da configuracao 2\n\n";
    
    // Configuracao 3: 
    // N = 1000; C_A = 0.05; C_B = 0.025
    // Apos testes, uso M = 10000
    std::cout << "Iniciada simulacao da configuracao 3\n";
    modelo_completo("resultados brutos/config-3.csv", "resultados consolidados (medias)/config-3.csv", 100, 1000, 0.05, 0.025, 10000);
    std::cout << "Concluida simulacao da configuracao 3\n\n";
    
    // Configuracao 4: 
    // N = 1000; C_A = 0.05; C_B = 0.0005
    // Apos testes, uso M = 20000
    std::cout << "Iniciada simulacao da configuracao 4\n";
    modelo_completo("resultados brutos/config-4.csv", "resultados consolidados (medias)/config-4.csv", 100, 1000, 0.05, 0.0005, 20000);
    std::cout << "Concluida simulacao da configuracao 4\n\n";
    
    // Configuracao 5: 
    // N = 1000000; C_A = 0.5; C_B = 0.25
    // Apos testes, uso M = 1000000
    std::cout << "Iniciada simulacao da configuracao 5\n";
    modelo_completo("resultados brutos/config-5.csv", "resultados consolidados (medias)/config-5.csv", 100, 1000000, 0.5, 0.25, 10000000);
    std::cout << "Concluida simulacao da configuracao 5\n\n";

    // Configuracao 6: 
    // N = 1000000; C_A = 0.5; C_B = 0.005
    // Apos testes, uso M = 1000000
    std::cout << "Iniciada simulacao da configuracao 6\n";
    modelo_completo("resultados brutos/config-6.csv", "resultados consolidados (medias)/config-6.csv", 100, 1000000, 0.5, 0.005, 10000000);
    std::cout << "Concluida simulacao da configuracao 6\n\n";

    // Configuracao 7: 
    // N = 1000000; C_A = 0.05; C_B = 0.025
    // Apos testes, uso M = 1000000
    std::cout << "Iniciada simulacao da configuracao 7\n";
    modelo_completo("resultados brutos/config-7.csv", "resultados consolidados (medias)/config-7.csv", 100, 1000000, 0.05, 0.025, 10000000);
    std::cout << "Concluida simulacao da configuracao 7\n\n";

    // Configuracao 8: 
    // N = 1000000; C_A = 0.05; C_B = 0.0005
    // Apos testes, uso M = 1000000
    std::cout << "Iniciada simulacao da configuracao 8\n";
    modelo_completo("resultados brutos/config-8.csv", "resultados consolidados (medias)/config-8.csv", 100, 1000000, 0.05, 0.0005, 10000000);
    std::cout << "Concluida simulacao da configuracao 8\n\n";

    return 0;
}
