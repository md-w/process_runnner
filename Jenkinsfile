pipeline {
    agent {
        dockerfile {
            filename '.devcontainer/Dockerfile'
        }
    }
    stages {
        stage('PreCompile') {
            steps {
                sh 'pwd'
                // checkout([$class: 'GitSCM', branches: [[name: '*/master']], extensions: [], userRemoteConfigs: [[url: 'https://github.com/Microsoft/vcpkg.git'], [credentialsId: 'git_videonetics_pass', url: 'http://git.videonetics.com/vcpaas/vtpl_agent_2022.git']]])
            }
        }
    }
}